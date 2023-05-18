/*******************************************************************************
  EVSYS Peripheral Library

  Company:
    Microchip Technology Inc.

  File Name:
    plib_evsys.c

  Summary:
    EVSYS Source File

  Description:
    None

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
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

#include "plib_evsys.h"
#include "interrupts.h"

volatile static EVSYS_OBJECT evsys[1];

void EVSYS_Initialize( void )
{    /*Event Channel User Configuration*/
    EVSYS_REGS->EVSYS_USER[38] = EVSYS_USER_CHANNEL(0x1UL);

    /* Event Channel 0 Configuration */
    EVSYS_REGS->EVSYS_CHANNEL[0] = EVSYS_CHANNEL_EVGEN(52UL) | EVSYS_CHANNEL_PATH(2UL) | EVSYS_CHANNEL_EDGSEL(1UL) \
                                     | EVSYS_CHANNEL_ONDEMAND_Msk;


    /*Interrupt setting for Event System*/
    EVSYS_REGS->EVSYS_INTENSET = 0x10000U;
}

void EVSYS_GeneratorEnable(EVSYS_CHANNEL channel, uint8_t generator)
{
   EVSYS_REGS->EVSYS_CHANNEL[channel] = (EVSYS_REGS->EVSYS_CHANNEL[channel] & ~EVSYS_CHANNEL_EVGEN_Msk) | EVSYS_CHANNEL_EVGEN(generator);
}

void EVSYS_GeneratorDisable(EVSYS_CHANNEL channel)
{
   EVSYS_REGS->EVSYS_CHANNEL[channel] = (EVSYS_REGS->EVSYS_CHANNEL[channel] & ~EVSYS_CHANNEL_EVGEN_Msk);
}

void EVSYS_UserEnable(EVSYS_CHANNEL channel, uint8_t user)
{
   EVSYS_REGS->EVSYS_USER[user] = EVSYS_USER_CHANNEL((channel + 1U));
}

void EVSYS_UserDisable(uint8_t user)
{
   EVSYS_REGS->EVSYS_USER[user] = 0x0U;
}


void EVSYS_InterruptEnable(EVSYS_CHANNEL channel, EVSYS_INT_MASK interruptMask)
{
   EVSYS_REGS->EVSYS_INTENSET = (uint32_t)interruptMask << (uint32_t)channel;
}

void EVSYS_InterruptDisable(EVSYS_CHANNEL channel, EVSYS_INT_MASK interruptMask)
{
   EVSYS_REGS->EVSYS_INTENCLR = (uint32_t)interruptMask << (uint32_t)channel;
}

void EVSYS_CallbackRegister(EVSYS_CHANNEL channel, EVSYS_CALLBACK callback, uintptr_t context )
{
   evsys[channel].callback = callback;
   evsys[channel].context = context;
}

void __attribute__((used)) EVSYS_InterruptHandler( void )
{
    uint8_t currentChannel = 0U;
    uint32_t eventIntFlagStatus = 0U;
    uint32_t overrunIntFlagStatus = 0U;

    uintptr_t context_var;

    /* Find any triggered channels, run associated callback handlers */
    for (currentChannel = 0U; currentChannel < 1U; currentChannel++)
    {

        /* Read the interrupt flag status */
        overrunIntFlagStatus = EVSYS_REGS->EVSYS_INTFLAG & ((uint32_t)EVSYS_INT_OVERRUN << currentChannel);
        eventIntFlagStatus = EVSYS_REGS->EVSYS_INTFLAG & ((uint32_t)EVSYS_INT_EVENT_DETECT << currentChannel);

        if (0U != (eventIntFlagStatus | overrunIntFlagStatus))
        {
            /* Find any associated callback entries in the callback table */
            if (evsys[currentChannel].callback != NULL)
            {
                context_var = evsys[currentChannel].context;
                evsys[currentChannel].callback(((eventIntFlagStatus | overrunIntFlagStatus) >> currentChannel), context_var);
            }

            /* Clear interrupt flag */
            EVSYS_REGS->EVSYS_INTFLAG = ((uint32_t)EVSYS_INT_OVERRUN  | (uint32_t)EVSYS_INT_EVENT_DETECT) << currentChannel;
        }
    }
}
