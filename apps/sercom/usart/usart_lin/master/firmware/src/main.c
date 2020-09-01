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
/*
 * This demo shows communication between a LIN Master and a LIN slave.
 * LIN master requests Switch and LED status from the LIN slave every 250 milliseconds.
 * <break><0x55><0x80> .. <0xYY><CRC>  where YY byte contains status of Switch (D0) and LED (D1)
 * If the switch status as indicated by the LIN slave is pressed then, LIN master
 * sends a request to toggle the LED on the LIN slave.
 * <break><0x55><0xC1><0xYY><CRC> where YY = 0x01 to turn on LED and YY = 0x00 to turn off LED  
 */

uint8_t txData[1];
uint8_t txBuffer[10];
uint8_t rxBuffer[10];

#define READ_SWITCH_LED_PID     0x80
#define WRITE_LED_PID           0xC1

uint16_t checksum = 0;
uint8_t nTxBytes = 0;

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

void LIN_MasterTransmit(uint8_t pid, uint8_t* pData, uint8_t nBytes)
{
    nTxBytes = 0;       
    
    txBuffer[nTxBytes++] = pid;
    
    while (nBytes--)
    {
        txBuffer[nTxBytes++] = *pData++;
    }    
               
    /* Calculate checksum */
    checksum = 0;
    LIN_MasterChecksumAdd(txBuffer, nTxBytes);
    txBuffer[nTxBytes++] = LIN_MasterChecksumGet();
              
    /* Transmit- PID + DATA + CHECKSUM */
    while (SERCOM0_USART_LIN_CommandSet(USART_LIN_MASTER_CMD_AUTO_TRANSMIT) == false);
    
    SERCOM0_USART_Write(txBuffer, nTxBytes);
}

void LIN_MasterReceive(uint8_t pid)
{
    while (SERCOM0_USART_LIN_CommandSet(USART_LIN_MASTER_CMD_AUTO_TRANSMIT) == false);
    
    txBuffer[0] = pid;    
    SERCOM0_USART_Write(txBuffer, 1);                 
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

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    SYSTICK_TimerStart();
    
    /* Wait for the switch press to start off the operations */
    while(SWITCH_Get() == 1);       

    /* Clear off the receive buffer from any unwanted data */
    SERCOM0_USART_Read(rxBuffer, SERCOM0_USART_ReadCountGet());
    
    while (1)
    {
        SYSTICK_DelayMs(250);                        
        
        LIN_MasterReceive(READ_SWITCH_LED_PID);
        
        /* Read out the local echo */
        LIN_MasterLocalEchoRead(1);
        
        /* Read slave response byte - 1 data byte and 1 checksum byte */
        while (SERCOM0_USART_ReadCountGet() < 2);
        
        SERCOM0_USART_Read(rxBuffer, 2);
        
        /* Verify the received checksum against calculated checksum */
        checksum = 0;
        LIN_MasterChecksumAdd(rxBuffer, 1);
        
        if (rxBuffer[1] == LIN_MasterChecksumGet())
        {        
            /* Bit position 0 contains switch status, Bit position 1 contains LED status */
            if ((rxBuffer[0] & 0x01) == 0)
            {
                /* As the switch is pressed, ask LIN slave to toggle its LED */
                txData[0] = (((rxBuffer[0] & 0x02) >> 1) ^ 0x01);            

                LIN_MasterTransmit(WRITE_LED_PID, txData, 1);
                
                /* Read out the local echo */
                LIN_MasterLocalEchoRead(nTxBytes);
            }   
        }
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

