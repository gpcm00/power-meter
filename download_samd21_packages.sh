#!/bin/bash

SAMD21_DFP_URL="https://packs.download.microchip.com/Microchip.SAMD21_DFP.3.7.262.atpack"

[ -f "Microchip.SAMD21_DFP.3.7.262.atpack" ] || wget $SAMD21_DFP_URL
if [ $? -ne 0 ]; then
    echo "failed to get SAMD21_DFP package"
    exit 1
fi

[ -d "samd21a" ] || unzip Microchip.SAMD21_DFP.3.7.262.atpack "samd21a/*"