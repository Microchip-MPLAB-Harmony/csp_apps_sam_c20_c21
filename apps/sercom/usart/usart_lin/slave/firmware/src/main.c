/*******************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include "definitions.h"                // SYS function prototypes


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
uint8_t rxBuffer[10];
uint8_t txBuffer[10];
bool ledStatus = false;
uint16_t checksum = 0;
volatile bool breakDetected = false;

void LIN_SlaveChecksumAdd(uint8_t* pBuffer, uint8_t nBytes)
{
    uint8_t i;
    
    for (i = 0; i <nBytes; i++)
    {
        checksum += pBuffer[i];
    }
}

uint8_t LIN_SlaveChecksumGet(void)
{             
    checksum = 0xFF ^ ((checksum & 0x00FF) + (checksum >> 8));
    return (uint8_t)checksum;
}

void LIN_SlaveLocalEchoRead(uint8_t nBytes)
{
    while (SERCOM0_USART_ReadCountGet() < nBytes);
        
    SERCOM0_USART_Read(rxBuffer, nBytes);

    if (memcmp(rxBuffer, txBuffer, nBytes) != 0)
    {
        /* handle error */        
    }
}

void SERCOM_RX_Handler(SERCOM_USART_EVENT event, uintptr_t context )
{
    if (event == SERCOM_USART_EVENT_BREAK_SIGNAL_DETECTED)
    {
        breakDetected = true;            
        
        /* Flush out any unwanted data in the receive buffer */
        SERCOM0_USART_Read(rxBuffer, SERCOM0_USART_ReadCountGet());
    }
}
int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    SERCOM0_USART_ReadCallbackRegister(SERCOM_RX_Handler, 0);
        
    while ( 1 )
    {
        if (breakDetected == true)
        {                        
            breakDetected = false;
            
            /* Read out the PID byte */
            while (SERCOM0_USART_ReadCountGet() < 1);
            
            SERCOM0_USART_Read(rxBuffer, 1);
            
            if (rxBuffer[0] == 0x80)
            {
                /* LIN Master has requested Switch and LED status */
                txBuffer[0] = ((ledStatus << 1) | (SWITCH_Get()));
                
                checksum = 0;
                LIN_SlaveChecksumAdd(txBuffer, 1);
                txBuffer[1] = LIN_SlaveChecksumGet();
                
                SERCOM0_USART_Write(txBuffer, 2);
                
                LIN_SlaveLocalEchoRead(2);
            }
            else if (rxBuffer[0] == 0xC1)
            {
                /* LIN Master is writing LED status */
                
                /* Read out the LED status byte and the checksum */
                while (SERCOM0_USART_ReadCountGet() < 2);

                SERCOM0_USART_Read(&rxBuffer[1], 2);
                
                /* Verify the received checksum against calculated checksum */
                checksum = 0;
                LIN_SlaveChecksumAdd(rxBuffer, 2);
                if (rxBuffer[2] == LIN_SlaveChecksumGet())
                {
                    /* Check the LED state requested by LIN master */
                    if (rxBuffer[1] & 0x01)
                    {
                        /* Turn on LED */
                        LED_Clear();
                        ledStatus = 1;
                    }
                    else
                    {
                        /* Turn off LED */
                        LED_Set();
                        ledStatus = 0;
                    }                    
                }
            }            
        }        
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

