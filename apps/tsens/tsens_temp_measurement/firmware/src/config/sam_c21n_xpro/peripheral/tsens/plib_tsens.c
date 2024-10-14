/*******************************************************************************
  Temperature Sensor (TSENS) PLIB

  Company
    Microchip Technology Inc.

  File Name
    plib_tsens.c

  Summary
    TSENS PLIB Implementation File.

  Description
    This file defines the interface to the TSENS peripheral library. This
    library provides access to and control of the associated peripheral
    instance.

  Remarks:
    None.

*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/* This section lists the other files that are included in this file.
*/

#include "interrupts.h"
#include "plib_tsens.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************
volatile static TSENS_CALLBACK_OBJ TSENS_CallbackObject;


// *****************************************************************************
// *****************************************************************************
// Section: TSENS Implementation
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Initialize TSENS module */
void TSENS_Initialize( void )
{
    /* Reset TSENS */
    TSENS_REGS->TSENS_CTRLA = (uint8_t)TSENS_CTRLA_SWRST_Msk;

    while((TSENS_REGS->TSENS_SYNCBUSY & TSENS_SYNCBUSY_SWRST_Msk) == TSENS_SYNCBUSY_SWRST_Msk)
    {
        /* Wait for Synchronization */
    }

    uint32_t calib_low_word = (uint32_t)(*(uint64_t*)TEMP_LOG_ADDR);
    uint32_t calib_high_word = (uint32_t)((*(uint64_t*)TEMP_LOG_ADDR) >> 32U);

    TSENS_REGS->TSENS_CAL = TSENS_CAL_TCAL(calib_low_word & FUSES_TEMP_LOG_WORD_0_TSENS_TCAL_Msk) |
        TSENS_CAL_FCAL((calib_low_word & FUSES_TEMP_LOG_WORD_0_TSENS_FCAL_Msk) >> FUSES_TEMP_LOG_WORD_0_TSENS_FCAL_Pos );
    
    TSENS_REGS->TSENS_GAIN = ((calib_high_word & 0xFU) << 20U) | ((calib_low_word & FUSES_TEMP_LOG_WORD_0_TSENS_GAIN_0_Msk) >> FUSES_TEMP_LOG_WORD_0_TSENS_GAIN_0_Pos ) ;
    TSENS_REGS->TSENS_OFFSET = calib_high_word >> FUSES_TEMP_LOG_WORD_1_TSENS_OFFSET_Pos;
    
    /* Resolution & Operation Mode */
    TSENS_REGS->TSENS_CTRLC = TSENS_CTRLC_WINMODE(0UL) ;
    /* Clear all interrupt flags */
    TSENS_REGS->TSENS_INTFLAG = (uint8_t)TSENS_INTFLAG_Msk;
    /* Enable interrupts */
    TSENS_REGS->TSENS_INTENSET = (uint8_t)(TSENS_INTENSET_RESRDY_Msk);
    /* Events configuration  */
    TSENS_REGS->TSENS_EVCTRL = (uint8_t)(TSENS_EVCTRL_STARTEI_Msk);

    while(0U != TSENS_REGS->TSENS_SYNCBUSY)
    {
        /* Wait for Synchronization */
    }
}

/* Enable TSENS module */
void TSENS_Enable( void )
{
    TSENS_REGS->TSENS_CTRLA |= (uint8_t)TSENS_CTRLA_ENABLE_Msk;
    while((TSENS_REGS->TSENS_SYNCBUSY & TSENS_SYNCBUSY_ENABLE_Msk) == TSENS_SYNCBUSY_ENABLE_Msk)
    {
        /* Wait for Synchronization */
    }
}

/* Disable TSENS module */
void TSENS_Disable( void )
{
    TSENS_REGS->TSENS_CTRLA &= (uint8_t)(~TSENS_CTRLA_ENABLE_Msk);
    while((TSENS_REGS->TSENS_SYNCBUSY & TSENS_SYNCBUSY_ENABLE_Msk) == TSENS_SYNCBUSY_ENABLE_Msk)
    {
        /* Wait for Synchronization */
    }
}

/* Start the TSENS conversion by SW */
void TSENS_ConversionStart( void )
{
    /* Start conversion */
    TSENS_REGS->TSENS_CTRLB = (uint8_t)TSENS_CTRLB_START_Msk;

}


/* Configure window comparison threshold values */
void TSENS_ComparisonWindowSet(uint32_t low_threshold, uint32_t high_threshold)
{
    TSENS_REGS->TSENS_WINLT = low_threshold;
    TSENS_REGS->TSENS_WINUT = high_threshold;
}

void TSENS_WindowModeSet(TSENS_WINMODE mode)
{
    TSENS_REGS->TSENS_CTRLC &= (uint8_t)(~TSENS_CTRLC_WINMODE_Msk);
    TSENS_REGS->TSENS_CTRLC |= (uint8_t)((uint32_t)mode << TSENS_CTRLC_WINMODE_Pos);
}

/* Read the conversion result */
uint32_t TSENS_ConversionResultGet( void )
{
    return (uint32_t)TSENS_REGS->TSENS_VALUE;
}

void TSENS_InterruptsClear(TSENS_STATUS interruptMask)
{
    TSENS_REGS->TSENS_INTFLAG = (uint8_t)interruptMask;
}

void TSENS_InterruptsEnable(TSENS_STATUS interruptMask)
{
    TSENS_REGS->TSENS_INTENSET = (uint8_t)interruptMask;
}

void TSENS_InterruptsDisable(TSENS_STATUS interruptMask)
{
    TSENS_REGS->TSENS_INTENCLR = (uint8_t)interruptMask;
}


/* Register callback function */
void TSENS_CallbackRegister( TSENS_CALLBACK callback, uintptr_t context )
{
    TSENS_CallbackObject.callback = callback;

    TSENS_CallbackObject.context = context;
}


void __attribute__((used)) TSENS_InterruptHandler( void )
{
    TSENS_STATUS status;
    status = TSENS_REGS->TSENS_INTFLAG;
    /* Clear interrupt flag */
    TSENS_REGS->TSENS_INTFLAG = (uint8_t)(TSENS_INTENSET_RESRDY_Msk);
    if (TSENS_CallbackObject.callback != NULL)
    {
        uintptr_t context = TSENS_CallbackObject.context;
        TSENS_CallbackObject.callback(status, context);
    }
}
