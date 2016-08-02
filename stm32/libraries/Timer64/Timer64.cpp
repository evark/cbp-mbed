 /*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include "Timer64.h"
#include "ticker_api.h"
#include "us_ticker_api.h"

Timer64::Timer64() :
    _timerInitialized(false),
    _timerRunning(false), 
    _tickerStartTimeUsec(0u), 
    _totalTimeUsec(0llu), 
    _ticker_data(get_us_ticker_data()),
    _rollOverCheckTimer(NULL),
    _rollOverCheckTimerPeriodInMsec(TIMER64_DEFAULT_ROLLOVER_CHECK_IN_MSECS),
    _sem(NULL)
{
    ;
}

Timer64::~Timer64()
{
    release();
}

int Timer64::init(uint32_t rolloverCheckTimeInMsec)
{
    if (_timerInitialized)
    {
        return(TIMER64_WARNING_ALREADY_INITIALIZED);
    }
    
    if (rolloverCheckTimeInMsec < TIMER64_MIN_ROLLOVER_CHECK_IN_MSECS)
    {
        rolloverCheckTimeInMsec = TIMER64_MIN_ROLLOVER_CHECK_IN_MSECS;
    }
    else if (rolloverCheckTimeInMsec > TIMER64_MAX_ROLLOVER_CHECK_IN_MSECS)
    {
        rolloverCheckTimeInMsec = TIMER64_MAX_ROLLOVER_CHECK_IN_MSECS;
    }

    _timerRunning = false; 
    _tickerStartTimeUsec = 0u; 
    _totalTimeUsec = 0llu; 
    _rollOverCheckTimerPeriodInMsec = rolloverCheckTimeInMsec;
    _rollOverCheckTimer = new RtosTimer(_rollOverCheck, osTimerPeriodic, this);
    _sem = new Semaphore(1);
    _timerInitialized = true;
    
    return(TIMER64_OK);
}

int Timer64::release(void)
{
    if (!_timerInitialized)
    {
        return(TIMER64_WARNING_ALREADY_RELEASED);
    }
    
    if (_timerRunning)
    {
        stop();
    }
    
    _tickerStartTimeUsec = 0u; 
    _totalTimeUsec = 0llu; 
    _timerInitialized = false;
    _rollOverCheckTimer->stop();
    delete _rollOverCheckTimer;
    _rollOverCheckTimer = NULL;
    delete _sem;
    _sem = NULL;
    
    return(TIMER64_OK);
}

int Timer64::start(void)
{
    int status = TIMER64_OK;

    _sem->wait();
    {
        if (!_timerInitialized)
        {
            status = TIMER64_ERROR_NOT_INITIALIZED;
        }
        else if (_timerRunning)
        {
            status = TIMER64_WARNING_ALREADY_RUNNING;
        }
        else
        {
            _tickerStartTimeUsec = ticker_read(_ticker_data);
            _timerRunning = true;
            _rollOverCheckTimer->start(_rollOverCheckTimerPeriodInMsec);
        }
    }
    _sem->release();
    
    return(status);
}

int Timer64::stop(void)
{
    int status = TIMER64_OK;

    _sem->wait();
    {
        if (!_timerInitialized)
        {
            status = TIMER64_ERROR_NOT_INITIALIZED;
        }
        else if (!_timerRunning)
        {
            status = TIMER64_WARNING_ALREADY_STOPPED;
        }
        else
        {
            _read_us(this);
            _timerRunning = false;
            _rollOverCheckTimer->stop();
        }
    }
    _sem->release();
    
    return(status);
}

int Timer64::reset(void)
{
    int status = TIMER64_OK;

    _sem->wait();
    {
        if (!_timerInitialized)
        {
            status = TIMER64_ERROR_NOT_INITIALIZED;
        }
        else
        {
            if (_timerRunning)
            {
                _tickerStartTimeUsec = ticker_read(_ticker_data);
                _rollOverCheckTimer->start(_rollOverCheckTimerPeriodInMsec);
            }
            
            _totalTimeUsec = 0llu;
        }
    }
    _sem->release();
    
    return(status);
}

int Timer64::read_us(uint64_t* timeInUsec)
{
    int status = TIMER64_OK;

    _sem->wait();
    {
        if (!_timerInitialized)
        {
            status = TIMER64_ERROR_NOT_INITIALIZED;
        }
        else if (timeInUsec == NULL)
        {
            status = TIMER64_ERROR_NULL_POINTER;
        }
        else
        {
            if (_timerRunning)
            {
                *timeInUsec = _read_us(this);
            }
            else
            {
                *timeInUsec = _totalTimeUsec;
            }
        }
    }
    _sem->release();
    
    return(status);
}

int Timer64::read_ms(uint64_t* timeInMsec)
{
    int status = TIMER64_OK;

    _sem->wait();
    {
        if (!_timerInitialized)
        {
            status = TIMER64_ERROR_NOT_INITIALIZED;
        }
        else if (timeInMsec == NULL)
        {
            status = TIMER64_ERROR_NULL_POINTER;
        }
        else
        {
            if (_timerRunning)
            {
                *timeInMsec = _read_us(this)/1000LU;
            }
            else
            {
                *timeInMsec = _totalTimeUsec/1000LU;
            }
        }
    }
    _sem->release();
    
    return(status);
}

int Timer64::read(double* timeInSec)
{
    int status = TIMER64_OK;

    _sem->wait();
    {
        if (!_timerInitialized)
        {
            status = TIMER64_ERROR_NOT_INITIALIZED;
        }
        else if (timeInSec == NULL)
        {
            status = TIMER64_ERROR_NULL_POINTER;
        }
        else
        {
            if (_timerRunning)
            {
                *timeInSec = (double)_read_us(this)/1000000.0L;
            }
            else
            {
                *timeInSec = (double)_totalTimeUsec/1000000.0L;
            }
        }
    }
    _sem->release();
    
    return(status);
}

int Timer64::isRunning(bool* running)
{
    int status = TIMER64_OK;

    _sem->wait();
    {
        if (!_timerInitialized)
        {
            status = TIMER64_ERROR_NOT_INITIALIZED;
        }
        else
        {
            *running = _timerRunning;
        }
    }
    _sem->release();
    
    return(status);
}

void Timer64::_rollOverCheck(void const* args)
{
    Timer64* timer = (Timer64*)args;
    timer->_read_us(timer);
    return;
}

uint64_t Timer64::_read_us(void const* args)
{
    Timer64* timer = (Timer64*)args;
    timestamp_t ticker_current_timeUsec = ticker_read(timer->_ticker_data);
    
    // check for ticker time rollover
    
    if (ticker_current_timeUsec >= timer->_tickerStartTimeUsec)
    {
        timer->_totalTimeUsec += (uint64_t)(ticker_current_timeUsec - timer->_tickerStartTimeUsec);
    }
    else
    {
        // rollover!
        timer->_totalTimeUsec += (uint64_t)(4294967296U - timer->_tickerStartTimeUsec) + (uint64_t)ticker_current_timeUsec;
        
    }
    
    timer->_rollOverCheckTimer->start(timer->_rollOverCheckTimerPeriodInMsec);
    timer->_tickerStartTimeUsec = ticker_current_timeUsec;
    return(timer->_totalTimeUsec);
}

