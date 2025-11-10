#!/bin/bash

SAMD21_DFP_URL="https://packs.download.microchip.com/Microchip.SAMD21_DFP.3.7.262.atpack"
ARM_CMSIS_URL="https://www.keil.com/pack/ARM.CMSIS.6.2.0.pack"

[ -f "Microchip.SAMD21_DFP.3.7.262.atpack" ] || wget $SAMD21_DFP_URL
if [ $? -ne 0 ]; then
    echo "Failed to get SAMD21_DFP package"
    exit 1
fi

[ -d "samd21a" ] || unzip Microchip.SAMD21_DFP.3.7.262.atpack "samd21a/*"

[ -f "ARM.CMSIS.6.2.0.pack" ] || wget $ARM_CMSIS_URL
if [ $? -ne 0 ]; then
    echo "Failed to get ARM_CMSIS package"
    exit 1
fi

[ -d "arm-core" ] || unzip ARM.CMSIS.6.2.0.pack -d "arm-core"