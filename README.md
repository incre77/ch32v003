Reset program and bin for ch32v003j4m6
------------------------------------------
This is a simple sketch to send a 200ms pulse thru the 5pin PA1 to any other reset pin device.
The normal cycle is 24 hours but you can change that seting some jumpers.

Instructions in pdf.

The programmer device i used is the official WCH-LinkE (5€ in aliexpress). Use only 3 pins from there 3.3v to vcc, gnd-gnd and SWDIO to 8pin in the ch32v003j4m6

Use this to recover a bricked chip: https://pypi.org/project/rvprog/

rvprog -b  

Use this to write the bin direcly to the chip:

rvprog -f pulso_reset.ino.bin

<img width="7795" height="3638" alt="image" src="https://github.com/user-attachments/assets/608fcc61-8be4-4149-a37f-94b47993b3e4" />





