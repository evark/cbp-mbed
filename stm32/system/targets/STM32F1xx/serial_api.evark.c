/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2014, STMicroelectronics
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************
 */
#include "mbed_assert.h"
#include "serial_api.h"

#if DEVICE_SERIAL

#include "cmsis.h"
#include "pinmap.h"
#include <string.h>
#include "PeripheralPins.h"
#include "mbed_error.h"

#define DEBUG_STDIO 0

#ifndef DEBUG_STDIO
#   define DEBUG_STDIO 0
#endif

#if DEBUG_STDIO
#   include <stdio.h>
#   define DEBUG_PRINTF(...) do { printf(__VA_ARGS__); } while(0)
#else
#   define DEBUG_PRINTF(...) {}
#endif

#define UART_NUM (3)

static uint32_t serial_irq_ids[UART_NUM] = {0, 0, 0};
static uart_irq_handler irq_handler;

static UART_HandleTypeDef UartHandle[UART_NUM];

int stdio_uart_inited = 0;
serial_t stdio_uart;

#define SERIAL_OBJ(X) (obj->X)

static void init_uart(serial_t *obj, UARTName instance)
{

    UART_HandleTypeDef *handle = &UartHandle[SERIAL_OBJ(index)];
    handle->Instance = (USART_TypeDef *)instance;

    handle->Init.BaudRate     = SERIAL_OBJ(baudrate);
    handle->Init.WordLength   = SERIAL_OBJ(databits);
    handle->Init.StopBits     = SERIAL_OBJ(stopbits);
    handle->Init.Parity       = SERIAL_OBJ(parity);
    handle->Init.HwFlowCtl    = UART_HWCONTROL_NONE;

    if (SERIAL_OBJ(pin_rx) == NC) {
      handle->Init.Mode = UART_MODE_TX;
    } else if (SERIAL_OBJ(pin_tx) == NC) {
      handle->Init.Mode = UART_MODE_RX;
    } else {
      handle->Init.Mode = UART_MODE_TX_RX;
    }

    /* uAMR & ARM: Call to UART init is done between reset of pre-initialized variables */
	/* and before HAL Init. SystemCoreClock init required here */
    SystemCoreClockUpdate();

    if (HAL_UART_Init(handle) != HAL_OK) {
        error("Cannot initialize UART\n");
    }

}

void serial_init(serial_t *obj, PinName tx, PinName rx)
{
    // Determine the UART to use (UART_1, UART_2, ...)
    UARTName uart_tx = (UARTName)pinmap_peripheral(tx, PinMap_UART_TX);
    UARTName uart_rx = (UARTName)pinmap_peripheral(rx, PinMap_UART_RX);

    // Get the peripheral name (UART_1, UART_2, ...) from the pin and assign it to the object
    UARTName instance = (UARTName)pinmap_merge(uart_tx, uart_rx);

    MBED_ASSERT(instance != (UARTName)NC);

    // Enable USART clock
    switch (instance) {
        case UART_1:
            __HAL_RCC_USART1_FORCE_RESET();
            __HAL_RCC_USART1_RELEASE_RESET();
            __HAL_RCC_USART1_CLK_ENABLE();
            SERIAL_OBJ(index) = 0;
            break;

        case UART_2:
            __HAL_RCC_USART2_FORCE_RESET();
            __HAL_RCC_USART2_RELEASE_RESET();
            __HAL_RCC_USART2_CLK_ENABLE();
            SERIAL_OBJ(index) = 1;
            break;

        case UART_3:
            __HAL_RCC_USART3_FORCE_RESET();
            __HAL_RCC_USART3_RELEASE_RESET();
            __HAL_RCC_USART3_CLK_ENABLE();
            SERIAL_OBJ(index) = 2;
            break;
    }

    // Configure the UART pins
    pinmap_pinout(tx, PinMap_UART_TX);
    pinmap_pinout(rx, PinMap_UART_RX);

    if (tx != NC) {
        pin_mode(tx, PullUp);
    }
    if (rx != NC) {
        pin_mode(rx, PullUp);
    }

    // Configure UART
    SERIAL_OBJ(baudrate) = 9600;
    SERIAL_OBJ(databits) = UART_WORDLENGTH_8B;
    SERIAL_OBJ(stopbits) = UART_STOPBITS_1;
    SERIAL_OBJ(parity)   = UART_PARITY_NONE;

    SERIAL_OBJ(pin_tx) = tx;
    SERIAL_OBJ(pin_rx) = rx;

    init_uart(obj, instance);

    // For stdio management
    if ((int)(UartHandle[SERIAL_OBJ(index)].Instance) == STDIO_UART) {
        stdio_uart_inited = 1;
        memcpy(&stdio_uart, obj, sizeof(serial_t));
    }

    DEBUG_PRINTF("UART%u: Init\n", obj->serial.module+1);
}

void serial_free(serial_t *obj)
{
    // Reset UART and disable clock
    switch (SERIAL_OBJ(index)) {
        case 0:
            __USART1_FORCE_RESET();
            __USART1_RELEASE_RESET();
            __USART1_CLK_DISABLE();
            break;
        case 1:
            __USART2_FORCE_RESET();
            __USART2_RELEASE_RESET();
            __USART2_CLK_DISABLE();
            break;
        case 2:
            __USART3_FORCE_RESET();
            __USART3_RELEASE_RESET();
            __USART3_CLK_DISABLE();
            break;
    }

    // Configure GPIOs
    pin_function(SERIAL_OBJ(pin_tx), STM_PIN_DATA(STM_MODE_INPUT, GPIO_NOPULL, 0));
    pin_function(SERIAL_OBJ(pin_rx), STM_PIN_DATA(STM_MODE_INPUT, GPIO_NOPULL, 0));

    serial_irq_ids[SERIAL_OBJ(index)] = 0;

    DEBUG_PRINTF("UART%u: Free\n", obj->serial.module+1);
}

void serial_baud(serial_t *obj, int baudrate)
{
    UART_HandleTypeDef *handle = &UartHandle[SERIAL_OBJ(index)];

    SERIAL_OBJ(baudrate) = baudrate;
    handle->Init.BaudRate = baudrate;

    if (HAL_UART_Init(handle) != HAL_OK) {
           error("Cannot initialize UART\n");
    }

    DEBUG_PRINTF("UART%u: Baudrate: %u\n", obj->serial.module+1, baudrate);
}

void serial_format(serial_t *obj, int data_bits, SerialParity parity, int stop_bits)
{
    UART_HandleTypeDef *handle = &UartHandle[SERIAL_OBJ(index)];

    if (data_bits == 9) {
        SERIAL_OBJ(databits) = UART_WORDLENGTH_9B;
        handle->Init.WordLength = UART_WORDLENGTH_9B;
    } else {
        SERIAL_OBJ(databits) = UART_WORDLENGTH_8B;
        handle->Init.WordLength = UART_WORDLENGTH_8B;
    }

    switch (parity) {
        case ParityOdd:
            SERIAL_OBJ(parity) = UART_PARITY_ODD;
            handle->Init.Parity = UART_PARITY_ODD;
            break;
        case ParityEven:
            SERIAL_OBJ(parity) = UART_PARITY_EVEN;
            handle->Init.Parity = UART_PARITY_EVEN;
            break;
        default: // ParityNone
        case ParityForced0: // unsupported!
        case ParityForced1: // unsupported!
            SERIAL_OBJ(parity) = UART_PARITY_NONE;
            handle->Init.Parity = UART_PARITY_NONE;
            break;
    }

    if (stop_bits == 2) {
        SERIAL_OBJ(stopbits) = UART_STOPBITS_2;
        handle->Init.StopBits = UART_STOPBITS_2;
    } else {
        SERIAL_OBJ(stopbits) = UART_STOPBITS_1;
        handle->Init.StopBits = UART_STOPBITS_1;
    }

    if (HAL_UART_Init(handle) != HAL_OK) {
           error("Cannot initialize UART\n");
    }

    DEBUG_PRINTF("UART%u: Format: %u, %u, %u\n", obj->serial.module+1, data_bits, parity, stop_bits);

}

/******************************************************************************
 * INTERRUPTS HANDLING
 ******************************************************************************/

static void uart_irq(int id)
{
  UART_HandleTypeDef *handle = &UartHandle[id];
    if (serial_irq_ids[id] != 0) {
        if (__HAL_UART_GET_FLAG(handle, UART_FLAG_TC) != RESET) {
            irq_handler(serial_irq_ids[id], TxIrq);
            __HAL_UART_CLEAR_FLAG(handle, UART_FLAG_TC);
        }
        if (__HAL_UART_GET_FLAG(handle, UART_FLAG_RXNE) != RESET) {
            irq_handler(serial_irq_ids[id], RxIrq);
            __HAL_UART_CLEAR_FLAG(handle, UART_FLAG_RXNE);
        }
        if (__HAL_UART_GET_FLAG(handle, UART_FLAG_ORE) != RESET) {
            uint8_t c = handle->Instance->DR;
        }
    }
}

static void uart1_irq(void)
{
    uart_irq(0);
}

static void uart2_irq(void)
{
    uart_irq(1);
}

static void uart3_irq(void)
{
    uart_irq(2);
}

void serial_irq_handler(serial_t *obj, uart_irq_handler handler, uint32_t id)
{
    irq_handler = handler;
    serial_irq_ids[SERIAL_OBJ(index)] = id;
}

void serial_irq_set(serial_t *obj, SerialIrq irq, uint32_t enable)
{
    IRQn_Type irq_n = (IRQn_Type)0;
    uint32_t vector = 0;

    UART_HandleTypeDef *handle = &UartHandle[SERIAL_OBJ(index)];

    switch (SERIAL_OBJ(index)) {
        case 0:
            irq_n = USART1_IRQn;
            vector = (uint32_t)&uart1_irq;
            break;
        case 1:
            irq_n = USART2_IRQn;
            vector = (uint32_t)&uart2_irq;
            break;
        case 2:
            irq_n = USART3_IRQn;
            vector = (uint32_t)&uart3_irq;
            break;
    }

    if (enable) {
        if (irq == RxIrq) {
            __HAL_UART_ENABLE_IT(handle, UART_IT_RXNE);
            NVIC_SetVector(irq_n, vector);
            NVIC_EnableIRQ(irq_n);
        } else { // TxIrq
            __HAL_UART_ENABLE_IT(handle, UART_IT_TC);
            NVIC_SetVector(irq_n, vector);
            NVIC_EnableIRQ(irq_n);
        }
    } else { // disable

        int all_disabled = 0;

        if (irq == RxIrq) {
            __HAL_UART_DISABLE_IT(handle, UART_IT_RXNE);
            // Check if TxIrq is disabled too
            if ((handle->Instance->CR1 & USART_CR1_TXEIE) == 0) all_disabled = 1;
        } else { // TxIrq
            __HAL_UART_DISABLE_IT(handle, UART_IT_TC);
            // Check if RxIrq is disabled too
            if ((handle->Instance->CR1 & USART_CR1_RXNEIE) == 0) all_disabled = 1;
        }

        if (all_disabled) {
          NVIC_DisableIRQ(irq_n);
        }

    }
}

/******************************************************************************
 * READ/WRITE
 ******************************************************************************/

int serial_getc(serial_t *obj)
{
    UART_HandleTypeDef *handle = &UartHandle[SERIAL_OBJ(index)];
    while (!serial_readable(obj));
    return (int)(handle->Instance->DR & 0x1FF);
}

void serial_putc(serial_t *obj, int c)
{
    UART_HandleTypeDef *handle = &UartHandle[SERIAL_OBJ(index)];
    while (!serial_writable(obj));
    handle->Instance->DR = (uint32_t)(c & 0x1FF);
}

int serial_readable(serial_t *obj)
{
    int status;
    UART_HandleTypeDef *handle = &UartHandle[SERIAL_OBJ(index)];
    // Check if data is received
    status = ((__HAL_UART_GET_FLAG(handle, UART_FLAG_RXNE) != RESET) ? 1 : 0);
    return status;
}

int serial_writable(serial_t *obj)
{
    int status;
    UART_HandleTypeDef *handle = &UartHandle[SERIAL_OBJ(index)];
    // Check if data is transmitted
    status = ((__HAL_UART_GET_FLAG(handle, UART_FLAG_TXE) != RESET) ? 1 : 0);
    return status;
}


void serial_clear(serial_t *obj)
{
    UART_HandleTypeDef *handle = &UartHandle[SERIAL_OBJ(index)];
    __HAL_UART_CLEAR_FLAG(handle, UART_FLAG_TXE);
    __HAL_UART_CLEAR_FLAG(handle, UART_FLAG_RXNE);
}


void serial_pinout_tx(PinName tx)
{
    pinmap_pinout(tx, PinMap_UART_TX);
}

void serial_break_set(serial_t *obj)
{
    UART_HandleTypeDef *uart = &UartHandle[SERIAL_OBJ(index)];
    HAL_LIN_SendBreak(uart);
}

void serial_break_clear(serial_t *obj)
{
    (void)obj;
}

#endif
