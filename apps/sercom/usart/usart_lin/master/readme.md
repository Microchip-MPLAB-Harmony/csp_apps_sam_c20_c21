---
parent: Harmony 3 peripheral library application examples for SAM C20/C21 family
title: SERCOM USART LIN Master 
has_children: false
has_toc: false
---

[![MCHP](https://www.microchip.com/ResourcePackages/Microchip/assets/dist/images/logo.png)](https://www.microchip.com)

# SERCOM USART LIN Master

This example application demonstrates how to use the SERCOM USART peripheral in LIN Master mode

## Description

In this example, the LIN Master reads LED and Switch status of the LIN slave every 250 milliseconds by sending a LIN receive command. The LIN slave responds with 1 byte data containing the Switch status (Bit[0]) and LED status (Bit[1]) followed by the Checksum byte. 

Upon receiving the response, LIN Master checks if the Switch status is in pressed or released state. If the switch is pressed, the LIN Master toggles the state of receieved LED status and asks the LIN slave to set the LED to the new state (set LED to OFF state if LED was previously ON and vice-versa) by sending a LIN transmit command.

PID for reading LED and Switch status - 0x80 (Frame ID - 0x00). 

PID for writing LED status - 0xC1 (Frame ID - 0x01). 

The example uses enhanced checksum.

## Downloading and building the application

To download or clone this application from Github, go to the [top level of the repository](https://github.com/Microchip-MPLAB-Harmony/csp_apps_sam_c20_c21) and click

![clone](../../../../docs/images/clone.png)

Path of the application within the repository is **apps/sercom/usart/usart_lin/master/firmware** .

To build the application, refer to the following table and open the project using its IDE.

| Project Name      | Description                                    |
| ----------------- | ---------------------------------------------- |
| sam_c21n_xpro.X | MPLABX project for [SAMC21N Xplained Pro Evaluation Kit](https://www.microchip.com/developmenttools/ProductDetails/atsamc21n-xpro) |
|||

## Setting up the hardware

The following table shows the target hardware for the application projects.

| Project Name| Board|
|:---------|:---------:|
| sam_c21n_xpro.X | [SAMC21N Xplained Pro Evaluation Kit](https://www.microchip.com/developmenttools/ProductDetails/atsamc21n-xpro)
|||

Hardware setup requires two boards, one acting as the LIN Master and the other as the LIN slave.

### Setting up [SAMC21N Xplained Pro Evaluation Kit](https://www.microchip.com/developmenttools/ProductDetails/atsamc21n-xpro)

- Enable Master node pull-up by mounting the jumper on J501 connector
- Mount the jumper on J500 connector to power the LIN transceiver from the on board VCC.
- Connect a wire from "LIN" on J503 on LIN Master board to "LIN" on J503 on LIN slave board
- Connect a wire from "GND" on J503 on LIN Master board to "GND" on J503 on LIN slave board
- Connect the Debug USB port on the board to the computer using a micro USB cable

## Running the Application

1. Build and Program LIN master and LIN slave applications using its IDE. Path of the LIN Slave application within the repository is **apps/sercom/usart/usart_lin/slave/firmware**.
2. Press switch on LIN Master board to start the communication. LIN Master will start reading LED and Switch status from the LIN slave every 250 milliseconds.
3. Press switch on the LIN Slave board. The LED on the LIN Slave board will toggle if the switch is pressed.
4. The following table provides the LED and Switch names

| Board      | LED Name  | Switch Name |
| ----------------- | ----------- | ----------- |
| [SAMC21N Xplained Pro Evaluation Kit](https://www.microchip.com/developmenttools/ProductDetails/atsamc21n-xpro) |LED0 | SW0 |
|||
