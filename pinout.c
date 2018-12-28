//*****************************************************************************
//
// Configure the device pins for different signals
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
// 
// 
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions 
//  are met:
//
//    Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the 
//    documentation and/or other materials provided with the   
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

// This file was automatically generated on 12/26/2018 at 10:12:51 AM
// by TI PinMux version 4.0.1530 
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "local_inc/pinout.h"

//*****************************************************************************
//
//! \addtogroup pinout_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! Configures the device pins for the customer specific usage.
//!
//! \return None.
//
//*****************************************************************************
void
PinoutSet(void)
{
    //
    // Enable Peripheral Clocks 
    //
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);

    //
    // Configure the GPIO Pin Mux for PE4
	// for GPIO_PE4
    //
	MAP_GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_4);

    //
    // Configure the GPIO Pin Mux for PC7
	// for GPIO_PC7
    //
	MAP_GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_7);

    //
    // Configure the GPIO Pin Mux for PH2
	// for GPIO_PH2
    //
	MAP_GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE, GPIO_PIN_2);

    //
    // Configure the GPIO Pin Mux for PM3
	// for GPIO_PM3
    //
	MAP_GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_3);

    //
    // Configure the GPIO Pin Mux for PN1
	// for GPIO_PN1
    //
	MAP_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

    //
    // Configure the GPIO Pin Mux for PN0
	// for GPIO_PN0
    //
	MAP_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

    //
    // Configure the GPIO Pin Mux for PF4
	// for GPIO_PF4
    //
	MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4);

    //
    // Configure the GPIO Pin Mux for PF0
	// for GPIO_PF0
    //
	MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);

    //
    // Configure the GPIO Pin Mux for PD1
	// for SSI2XDAT0
    //
	MAP_GPIOPinConfigure(GPIO_PD1_SSI2XDAT0);
	MAP_GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_1);

    //
    // Configure the GPIO Pin Mux for PD6
	// for SSI2XDAT3
    //
	MAP_GPIOPinConfigure(GPIO_PD6_SSI2XDAT3);
	MAP_GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_6);

    //
    // Configure the GPIO Pin Mux for PD0
	// for SSI2XDAT1
    //
	MAP_GPIOPinConfigure(GPIO_PD0_SSI2XDAT1);
	MAP_GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_0);

    //
    // Configure the GPIO Pin Mux for PD2
	// for SSI2FSS
    //
	MAP_GPIOPinConfigure(GPIO_PD2_SSI2FSS);
	MAP_GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_2);

	//
	// Unlock the Port Pin and Set the Commit Bit
	//
	HWREG(GPIO_PORTD_BASE+GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTD_BASE+GPIO_O_CR)   |= GPIO_PIN_7;
	HWREG(GPIO_PORTD_BASE+GPIO_O_LOCK) = 0x0;
	
    //
    // Configure the GPIO Pin Mux for PD7
	// for SSI2XDAT2
    //
	MAP_GPIOPinConfigure(GPIO_PD7_SSI2XDAT2);
	MAP_GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_7);

    //
    // Configure the GPIO Pin Mux for PD3
	// for SSI2CLK
    //
	MAP_GPIOPinConfigure(GPIO_PD3_SSI2CLK);
	MAP_GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_3);

    //
    // Configure the GPIO Pin Mux for PA4
	// for SSI0XDAT0
    //
	MAP_GPIOPinConfigure(GPIO_PA4_SSI0XDAT0);
	MAP_GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_4);

    //
    // Configure the GPIO Pin Mux for PA7
	// for SSI0XDAT3
    //
	MAP_GPIOPinConfigure(GPIO_PA7_SSI0XDAT3);
	MAP_GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_7);

    //
    // Configure the GPIO Pin Mux for PA5
	// for SSI0XDAT1
    //
	MAP_GPIOPinConfigure(GPIO_PA5_SSI0XDAT1);
	MAP_GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5);

    //
    // Configure the GPIO Pin Mux for PA3
	// for SSI0FSS
    //
	MAP_GPIOPinConfigure(GPIO_PA3_SSI0FSS);
	MAP_GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_3);

    //
    // Configure the GPIO Pin Mux for PA6
	// for SSI0XDAT2
    //
	MAP_GPIOPinConfigure(GPIO_PA6_SSI0XDAT2);
	MAP_GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_6);

    //
    // Configure the GPIO Pin Mux for PA2
	// for SSI0CLK
    //
	MAP_GPIOPinConfigure(GPIO_PA2_SSI0CLK);
	MAP_GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_2);

    //
    // Configure the GPIO Pin Mux for PA0
	// for U0RX
    //
	MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
	MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0);

    //
    // Configure the GPIO Pin Mux for PA1
	// for U0TX
    //
	MAP_GPIOPinConfigure(GPIO_PA1_U0TX);
	MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_1);

    //
    // Configure the GPIO Pin Mux for PM5
	// for U0DCD
    //
	MAP_GPIOPinConfigure(GPIO_PM5_U0DCD);
	MAP_GPIOPinTypeUART(GPIO_PORTM_BASE, GPIO_PIN_5);

    //
    // Configure the GPIO Pin Mux for PP2
	// for U0DTR
    //
	MAP_GPIOPinConfigure(GPIO_PP2_U0DTR);
	MAP_GPIOPinTypeUART(GPIO_PORTP_BASE, GPIO_PIN_2);

    //
    // Configure the GPIO Pin Mux for PB4
	// for U0CTS
    //
	MAP_GPIOPinConfigure(GPIO_PB4_U0CTS);
	MAP_GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_4);

    //
    // Configure the GPIO Pin Mux for PH3
	// for U0DSR
    //
	MAP_GPIOPinConfigure(GPIO_PH3_U0DSR);
	MAP_GPIOPinTypeUART(GPIO_PORTH_BASE, GPIO_PIN_3);

    //
    // Configure the GPIO Pin Mux for PB5
	// for U0RTS
    //
	MAP_GPIOPinConfigure(GPIO_PB5_U0RTS);
	MAP_GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_5);

    //
    // Configure the GPIO Pin Mux for PK7
	// for U0RI
    //
	MAP_GPIOPinConfigure(GPIO_PK7_U0RI);
	MAP_GPIOPinTypeUART(GPIO_PORTK_BASE, GPIO_PIN_7);

}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

