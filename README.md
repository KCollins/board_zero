# board_zero
Software files for using the Board Zero for data collection. 


## 2022-07-27-CCW_transmitter.9.2.ino
Sets up the transmit side of coherent CW. Illustrates how to declare a volatile variable that an interrupt can set; how to declare a hardware interrupt and attach it to an interrupt service routine. 

## Second_ticks_3.0.ino
Same things; set up to do data collection from audio output of a shortwave radio and transmit them, via a serial port, to python host code. 

## a_history.ino
Text file of the histories of the CCW transmit versions.

## c_GPS_interrupt.ino
Sets up the hardware interrupt service routine for the GPS 1 pulse per second.

## f_key_transmitter.ino
Sets up the software timer interrupt that does Morse code servicing. Send rate can be set. 
