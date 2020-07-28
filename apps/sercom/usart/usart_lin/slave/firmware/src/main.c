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

void LIN_MasterChecksumAdd(uint8_t* pBuffer, uint8_t nBytes)
{
    uint8_t i;
    
    for (i = 0; i <nBytes; i++)
    {
        checksum += pBuffer[i];
    }
}

uint8_t LIN_MasterChecksumGet(void)
{             
    checksum = 0xFF ^ ((checksum & 0x00FF) + (checksum >> 8));
    return (uint8_t)checksum;
}

void LIN_MasterLocalEchoRead(uint8_t nBytes)
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
                LIN_MasterChecksumAdd(txBuffer, 1);
                txBuffer[1] = LIN_MasterChecksumGet();
                
                SERCOM0_USART_Write(txBuffer, 2);
                
                LIN_MasterLocalEchoRead(2);
            }
            else if (rxBuffer[0] == 0xC1)
            {
                /* LIN Master is writing LED status */
                
                /* Read out the LED status byte and the checksum */
                while (SERCOM0_USART_ReadCountGet() < 2);

                SERCOM0_USART_Read(&rxBuffer[1], 2);
                
                /* Verify the received checksum against calculated checksum */
                checksum = 0;
                LIN_MasterChecksumAdd(rxBuffer, 2);
                if (rxBuffer[2] == LIN_MasterChecksumGet())
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

