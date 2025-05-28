/*!
    \file    gd32f10x_it.c
    \brief   interrupt service routines

    \version 2024-12-20, V2.5.0, firmware for GD32F10x
*/

/*
    Copyright (c) 2024, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/
#include "gd32f10x_it.h"
#include "usart_drv.h"
//#include "main.h"
#include "systick.h"
#include "elog.h"
#include "osal.h"
#include "osal_timer.h"

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
    /* if NMI exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
    /* if SVC exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
    /* if DebugMon exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
    /* if PendSV exception occurs, go to infinite loop */
    //while(1){
    //}
	OSAL_FLUSH_REALTIME_SIGNAL_ISR();
	OSAL_FLUSH_SIGNAL_ISR();
}

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
   // led_spark();
    delay_decrement();

    OSAL_TIMER_HANDLE();
}


/*!
    \brief      this function handles USART RBNE interrupt request and TBE interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USART0_IRQHandler(void)
{   
	  //static uint32_t rx_total_count = 0;
    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE)){
			 uint8_t rx_byte = 0;
			 uint32_t fifo_size = 0, watermark_flag = 0;

			 USART_RXFIFO_ENQUEUE_ISR(0, rx_byte);
			 //rx_total_count ++;
			 USART_RXFIFO_SIZE_ISR(0, fifo_size, watermark_flag);    
			 //log_i("rx_byte: %02X, fifo_size: %d, rx_total_count: %d, watermark_flag: %d", rx_byte, fifo_size, rx_total_count, watermark_flag);
			 if(watermark_flag)
			     OSAL_REALTIME_SIGNAL_EMIT_ISR(APP_SIGNAL_PELCOD, 1);                                                                                                    \
    }
    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_TBE)){
			   USART_TXFIFO_DEQUEUE_ISR(0);
    }
}


/*!
    \brief      this function handles USART RBNE interrupt request and TBE interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USART1_IRQHandler(void)
{
	  //static uint32_t rx_total_count = 0;
    if(RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_RBNE)){
			 uint8_t rx_byte = 0;
			 uint32_t fifo_size = 0, watermark_flag = 0;

			 USART_RXFIFO_ENQUEUE_ISR(1, rx_byte);
			 //rx_total_count ++;
			 USART_RXFIFO_SIZE_ISR(1, fifo_size, watermark_flag);    
			 //log_i("rx_byte: %02X, fifo_size: %d, rx_total_count: %d, watermark_flag: %d", rx_byte, fifo_size, rx_total_count, watermark_flag);
			 if(watermark_flag)
			     OSAL_REALTIME_SIGNAL_EMIT_ISR(APP_SIGNAL_PELCOD, 1);                                                                                                    \
    }
    if(RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_TBE)){
			   USART_TXFIFO_DEQUEUE_ISR(1);
    }
}

/*!
    \brief      this function handles USART RBNE interrupt request and TBE interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USART2_IRQHandler(void)
{
	  //static uint32_t rx_total_count = 0;
    if(RESET != usart_interrupt_flag_get(USART2, USART_INT_FLAG_RBNE)){
			 uint8_t rx_byte = 0;
			 uint32_t fifo_size = 0, watermark_flag = 0;
       static uint32_t rx_total_count = 0;
			 USART_RXFIFO_ENQUEUE_ISR(2, rx_byte);
			 rx_total_count ++;
			 USART_RXFIFO_SIZE_ISR(2, fifo_size, watermark_flag);    
			 //log_i("rx_byte: %02X, fifo_size: %d, rx_total_count: %d, watermark_flag: %d", rx_byte, fifo_size, rx_total_count, watermark_flag);
			 if(watermark_flag)
			     OSAL_REALTIME_SIGNAL_EMIT_ISR(APP_SIGNAL_PELCOD, 1);                                                                                                    \
    }
    if(RESET != usart_interrupt_flag_get(USART2, USART_INT_FLAG_TBE)){
			   USART_TXFIFO_DEQUEUE_ISR(2);
    }
}

/*!
    \brief      this function handles SPI0 Handler exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SPI0_IRQHandler(void)
{
//    if(RESET != spi_i2s_interrupt_flag_get(SPI0, SPI_I2S_INT_FLAG_TBE)){
//        /* send data */
//        spi_i2s_data_transmit(SPI0, spi0_send_array[send_n++]);

//        if(ARRAYSIZE == send_n){
//#if SPI_CRC_ENABLE
//            /* send the CRC value */
//            spi_crc_next(SPI0);
//#endif /* enable CRC function */
//            spi_i2s_interrupt_disable(SPI0, SPI_I2S_INT_TBE);
//        }
//    }
}

/*
    \brief      this function handles SPI0 Handler exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SPI1_IRQHandler(void)
{
 //   if(RESET != spi_i2s_interrupt_flag_get(SPI0, SPI_I2S_INT_FLAG_TBE)){
//        /* send data */
//        spi_i2s_data_transmit(SPI0, spi0_send_array[send_n++]);

//        if(ARRAYSIZE == send_n){
//#if SPI_CRC_ENABLE
//            /* send the CRC value */
//            spi_crc_next(SPI0);
//#endif /* enable CRC function */
//            spi_i2s_interrupt_disable(SPI0, SPI_I2S_INT_TBE);
//        }
//    }
}

/*
    \brief      this function handles SPI0 Handler exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SPI2_IRQHandler(void)
{
  //  if(RESET != spi_i2s_interrupt_flag_get(SPI0, SPI_I2S_INT_FLAG_TBE)){
//        /* send data */
//        spi_i2s_data_transmit(SPI0, spi0_send_array[send_n++]);

//        if(ARRAYSIZE == send_n){
//#if SPI_CRC_ENABLE
//            /* send the CRC value */
//            spi_crc_next(SPI0);
//#endif /* enable CRC function */
//            spi_i2s_interrupt_disable(SPI0, SPI_I2S_INT_TBE);
//        }
//    }
}


void RTC_IRQHandler(void)
{
    if (rtc_flag_get(RTC_FLAG_SECOND) != RESET){
        /* clear the RTC second interrupt flag*/
        rtc_flag_clear(RTC_FLAG_SECOND);
			  OSAL_REALTIME_SIGNAL_EMIT_ISR(OSAL_SIGNAL_KEEPALIVE, 1);                                                                                                    \

        /* enable time update */
       // timedisplay = 1;

        /* wait until last write operation on RTC registers has finished */
 //       rtc_lwoff_wait();
        /* reset RTC counter when time is 23:59:59 */
 //       if (rtc_counter_get() == 0x00015180){
   //         rtc_counter_set(0x0);
            /* wait until last write operation on RTC registers has finished */
  //          rtc_lwoff_wait();
    //    }
    }
}
