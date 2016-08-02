/* mbed Microcontroller Library
 * Copyright (c) 2006-2016 ARM Limited
 *
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
 *
 * Modified by Tom Doyle    <ThomasEDoyle@gmail.com>
 *
 */
 
#ifndef __TIMER64__
#define __TIMER64__

#include "platform.h"
#include "ticker_api.h"
#include <stdint.h>
#include "rtos.h"

#define TIMER64_OK                                  0

#define TIMER64_ERROR_NOT_INITIALIZED               -1
#define TIMER64_ERROR_NULL_POINTER                  -2

#define TIMER64_WARNING_ALREADY_INITIALIZED         1
#define TIMER64_WARNING_ALREADY_RUNNING             2
#define TIMER64_WARNING_ALREADY_STOPPED             3
#define TIMER64_WARNING_ALREADY_RELEASED            4

#define TIMER64_DEFAULT_ROLLOVER_CHECK_IN_MSECS     30000
#define TIMER64_MIN_ROLLOVER_CHECK_IN_MSECS         1000
#define TIMER64_MAX_ROLLOVER_CHECK_IN_MSECS         65535

/** Timer64 class
 *  Used to define a 64 bit timer64 that is thread safe.  The Timer64 behaves in a 
 *  similiar fashion to the mbed Timer class with the most notable exception that 
 *  the Timer64 will not rollover after 2^31 microseconds (approximately 35.8 minutes).
 *  The Timer64 extends the rollover time to 2^64 microseconds (approximately 585,000 years!)
 *
 *  The Timer64 class is also designed to be thread safe. The following functions can be 
 *  called from any thread after the class is instaniated and initialized:
 *  
 *  - start()
 *  - stop()
 *  - reset()
 *  - read_us()
 *  - read_ms()
 *  - read()
 *  - reset()
 *  - isRunning()
 *
 *  It is to be noted that the init() and release() methods need to be called from the same thread.
 *
 *  Give the Timer64 library a try and let me know if it works for you. Will gladly take suggestions
 *  on how to make it better.
 *
 *  Simple Example
 *  @code
 *  #include "mbed.h"
 *  #include "rtos.h"
 *  #include "Timer64.h"
 *
 * Timer64 timer64;
 *
 *   int main()
 *   {
 *      uint64_t timeInUsec;
 *      timer64.init();
 *      timer64.start();
 *      Thread::wait(100);
 *      timer64.stop();
 *      timer64.read_us(&timeInUsec);
 *      printf("Test - Elapsed time = %llu usec\n\n", timeInUsec);
 *      timer64.reset();
 *      timer64.release();
 *       
 *      while (1)
 *      {
 *          Thread::wait(1000);
 *      }
 *  }
 *  @endcode
*/

class Timer64
{

public:
    Timer64();
    ~Timer64();

    /** Initialize the timer - this must be called before the timer can be used
     *  
     *  @param rolloverCheckTimeInMsecc specifies how ofter a rollover check is performed.
     *      This parameter is bounded by TIMER64_MIN_ROLLOVER_CHECK_IN_MSECS and 
     *      TIMER64_MIN_ROLLOVER_CHECK_IN_MSECS with a default value of
     *      TIMER64_MIN_ROLLOVER_CHECK_IN_MSECS
     *
     *  @returns
     *      TIMER64_OK,
     *      TIMER64_WARNING_ALREADY_INITIALIZED
     */
    int init(uint32_t rolloverCheckTimeInMsecc = TIMER64_DEFAULT_ROLLOVER_CHECK_IN_MSECS);

    /** Release the timer - must be called from the same thread calling init()
     *  
     *  @returns
     *      TIMER64_OK,
     *      TIMER64_WARNING_ALREADY_RELEASED
     */
    int release(void);

    /** Start the timer - this method is thread safe
     *  
     *  @returns
     *      TIMER64_OK,
     *      TIMER64_ERROR_NOT_INITIALIZED,
     *      TIMER64_WARNING_ALREADY_RUNNING
     */
    int start(void);

    /** Stop the timer - this method is thread safe
     *  
     *  @returns
     *      TIMER64_OK,
     *      TIMER64_ERROR_NOT_INITIALIZED,
     *      TIMER64_WARNING_ALREADY_STOPPED
     */
    int stop(void);

    /** Reset the timer - this method is thread safe
     *  
     *  @returns
     *      TIMER64_OK,
     *      TIMER64_ERROR_NOT_INITIALIZED
     */
    int reset(void);

    /** Read the timer value in microseconds - this method is thread safe
     *  
     *  @param timeInUsec specifies a pointer to a uint64_t where to save the current time in microseconds
     *
     *  @returns
     *      TIMER64_OK,
     *      TIMER64_ERROR_NOT_INITIALIZED,
     *      TIMER64_ERROR_NULL_POINTER
     */
    int read_us(uint64_t* timeInUsec = NULL);

    /** Read the timer value in milliseconds - this method is thread safe
     *  
     *  @param timeInMsec specifies a pointer to a uint64_t where to save the current time in milliseconds
     *
     *  @returns
     *      TIMER64_OK,
     *      TIMER64_ERROR_NOT_INITIALIZED,
     *      TIMER64_ERROR_NULL_POINTER
     */
    int read_ms(uint64_t* timeInMsec = NULL);

    /** Read the timer value in seconds - this method is thread safe
     *  
     *  @param timeInMsec specifies a pointer to a double where to save the current time in seconds
     *
     *  @returns
     *      TIMER64_OK,
     *      TIMER64_ERROR_NOT_INITIALIZED,
     *      TIMER64_ERROR_NULL_POINTER,
     */
    int read(double* timeInSec = NULL);
    
    /** Check to see if timer is running - this method is thread safe
     *  
     *  @param running specifies a pointer to a bool where to save the running status
     *
     *  @returns
     *      TIMER64_OK,
     *      TIMER64_ERROR_NOT_INITIALIZED,
     *      TIMER64_ERROR_NULL_POINTER
     */
    int isRunning(bool* running);

private:
    bool _timerInitialized;
    bool _timerRunning;                 // whether the timer is running
    timestamp_t _tickerStartTimeUsec;   // the start time of the latest slice
    uint64_t _totalTimeUsec;            // any accumulated time from previous slices
    const ticker_data_t *_ticker_data;
    RtosTimer* _rollOverCheckTimer;
    uint32_t _rollOverCheckTimerPeriodInMsec;
    Semaphore* _sem;
    
    static void _rollOverCheck(void const* args);
    static uint64_t _read_us(void const* args);
};

#endif  //__TIMER64__
