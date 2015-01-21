#!/bin/sh

if [ $# -ne 2 ]
  then
    echo "not enough arguments supplied"
    echo "cpy_bin.sh [PATH_TO_BIN_FILE] [PATH_TO_TARGET_DIRECTORY]"
    exit 0
fi

if [ ! -e $1 ]; then
    echo "file not found!"
    exit 0
fi

echo "copy bin to usb"
#copy file $1 to stick $2 
sudo cp $1 $2

echo "remove bin from card"
#remove file $1 from card
sudo rm $1

