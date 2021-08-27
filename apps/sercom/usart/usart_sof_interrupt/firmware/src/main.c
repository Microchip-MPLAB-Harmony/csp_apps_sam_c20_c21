// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2021 Microchip Technology Inc. and its subsidiaries.
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
// DOM-IGNORE-END

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
#include <stdio.h>
#include "definitions.h"                // SYS function prototypes


#define txBufferLength 60

uint8_t rxBuffer[1];
uint8_t txBuffer[txBufferLength];

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    uint32_t nBytes = 0;
    
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    // Print the size of the read buffer on the terminal
    nBytes = sprintf((char*)txBuffer, "######################################################\n\r");
    SERCOM4_USART_Write ((uint8_t*)txBuffer, nBytes);  
    
    // Print the size of the write buffer on the terminal
    nBytes = sprintf((char*)txBuffer, "#      USART Start of Frame wake-up application      #\n\r");   
    SERCOM4_USART_Write ((uint8_t*)txBuffer, nBytes);
    
    // Print the size of the write buffer on the terminal
    nBytes = sprintf((char*)txBuffer, "######################################################\n\n\r");
    SERCOM4_USART_Write ((uint8_t*)txBuffer, nBytes);
    
    // Wait for all bytes to be transmitted out
    while (SERCOM4_USART_WriteCountGet() != 0);

    while ( true )
    {
        // Print the size of the write buffer on the terminal
        nBytes = sprintf((char *)txBuffer, "> Enter character to wake-up from sleep mode\n\r");
        SERCOM4_USART_Write ((uint8_t *)txBuffer, nBytes);
        
        // Wait for all bytes to be transmitted out
        while (SERCOM4_USART_WriteCountGet() != 0);
        
        SERCOM4_REGS->USART_INT.SERCOM_INTENCLR = SERCOM_USART_INT_INTENCLR_DRE_Msk;
        
        PM_StandbyModeEnter ();
        
        nBytes = sprintf ((char *)txBuffer, "Character received ! Device woke-up from sleep mode.");
        SERCOM4_USART_Write ((uint8_t *)txBuffer, nBytes);
        
        SERCOM4_USART_Write ((uint8_t *)"\n\n\r", 3);
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/