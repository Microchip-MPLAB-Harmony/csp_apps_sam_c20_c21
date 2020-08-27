---
parent: Harmony 3 peripheral library application examples for SAM C20/C21 family
title: ADC DMA sleepwalking 
has_children: false
has_toc: false
---

[![MCHP](https://www.microchip.com/ResourcePackages/Microchip/assets/dist/images/logo.png)](https://www.microchip.com)

# ADC DMA sleepwalking

This application shows how to configure the ADC in a sleepwalking operation, where the input voltage is measured at a fixed interval, and then the device is woken from sleep after conversion of 16 samples.

## Description

This example application shows how to use the ADC peripheral library to perform sleepwalking and how the DMA wakes up the device after conversion of N samples. ADC conversion is triggered by RTC compare 0 event. On every trigger, input voltage is converted and the result is copied to an array in SRAM by the DMA channel. All these peripherals are operated while the CPU is in sleep. DMA generates interrupt after it transfers 16 results into the array in SRAM, which wakes the CPU.

## Downloading and building the application

To clone or download this application from Github, go to the [main page of this repository](https://github.com/Microchip-MPLAB-Harmony/csp_apps_sam_c20_c21) and then click **Clone** button to clone this repository or download as zip file.
This content can also be downloaded using content manager by following these [instructions](https://github.com/Microchip-MPLAB-Harmony/contentmanager/wiki).

Path of the application within the repository is **apps/adc/adc_dmac_sleepwalking/firmware** .

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

### Setting up [SAMC21N Xplained Pro Evaluation Kit](https://www.microchip.com/developmenttools/ProductDetails/atsamc21n-xpro)

- Connect a voltage below the selected ADC reference voltage (VDDANA) to pin 4 (PB08 – ADC0_AIN2) of EXT1 connector
- Connect the Debug USB port on the board to the computer using a micro USB cable

## Running the Application

1. Open the Terminal application (Ex.:Tera term) on the computer
2. Connect to the EDBG Virtual COM port and configure the serial settings as follows:
    - Baud : 115200
    - Data : 8 Bits
    - Parity : None
    - Stop : 1 Bit
    - Flow Control : None
3. Build and Program the application using its IDE
4. CPU wakes up after every 16 transfers of ADC result and updates the console as shown below:

    ![output](images/output_adc_dma_sleepwalking.png)

5. Failure is indicated by turning ON the user LED (i.e. application failed if the LED is turned ON)

Refer to the table below for details of LED:

| Board| LED name|
|------|---------|
| [SAMC21N Xplained Pro Evaluation Kit](https://www.microchip.com/developmenttools/ProductDetails/atsamc21n-xpro) | LED0 |
|||
