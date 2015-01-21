#!/bin/sh

echo "copy bin to usb"
#copy file $1 to stick $2 
sudo cp $1 $2

echo "remove bin from card"
#remove file $1 from card
sudo rm $1

