---
parent: Harmony 3 peripheral library application examples for SAM C20/C21 family
title: NVMCTRL flash read while write (RWW) 
has_children: false
has_toc: false
---

[![MCHP](https://www.microchip.com/ResourcePackages/Microchip/assets/dist/images/logo.png)](https://www.microchip.com)

# NVMCTRL flash read while write (RWW)

This example application demonstrates code execution from the main Flash memory while performing erase operation in read-while-write section.

## Description

The main Flash memory can not be read while it is being erased or written, the CPU is stalled during the entire operation. Some devices has the Flash region that support read-while-write (RWW) feature. The user could execute code from main Flash while the the RWW region is being erased or written. This application demonstrates code execution from main Flash while the read-while-write section is being erased. A software counter is incremented while performing erase operation in read-while-write section and the counter value is reported in the console.

## Downloading and building the application

To clone or download this application from Github, go to the [main page of this repository](https://github.com/Microchip-MPLAB-Harmony/csp_apps_sam_c20_c21) and then click **Clone** button to clone this repository or download as zip file.
This content can also be downloaded using content manager by following these [instructions](https://github.com/Microchip-MPLAB-Harmony/contentmanager/wiki).

Path of the application within the repository is **apps/nvmctrl/nvmctrl_flash_read_while_write/firmware** .

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
4. The LED blinks continuously on success

    The following table provides the LED name.

    | Board Name | LED Name |
    |------------| -------- |
    |[SAMC21N Xplained Pro Evaluation Kit](https://www.microchip.com/developmenttools/ProductDetails/atsamc21n-xpro) | LED0 |

5. Observe output message in console as follows:

    ![output](images/output_nvmctrl_flash_read_while_write.png)