# Overview

This is the code files for the water robot project.

This is the radio transmitter being used:
NRF24L01+PA+LNA RF Transceiver Module with SMA Antenna 2.4 GHz 1100m
https://www.amazon.com/dp/B096DLGV8F?niid=nl_cl_lst_a_1_2&nrid=DHRBGRBR61HC6J5WK0NX

# Usage
To run the files in python, the pyrf24 library is needed:

` pip install pyrf24 `

For usage, you would have to load the arduino with the control script, then run both python scripts at the same time.

` python pi_transmitter.py `

` python pi_receiver.py `

# Documentation
pi_transmitter.py (Raspberry Pi Control Station)
This script runs on the transmitting Raspberry Pi. It handles the control logic, packaging motor commands (PWM values) into a compact, low-latency binary payload using Python's struct library, and sending them wirelessly via the NRF24L01 radio module. It currently runs a programmed test sequence (ramp up, hold, ramp down, rest) but is architected to support high-frequency updates like a PID control loop.

pi_receiver.py (Raspberry Pi Relay)
This script runs on the receiving Raspberry Pi. It actively listens for incoming binary radio packets, unpacks the byte payload back into motor integer values, formats them into a comma-separated string, and forwards them to the connected Arduino Micro via USB serial. It runs a continuous, high-speed loop to ensure minimal latency between the radio reception and the serial transmission.

arduino_controller.ino (Arduino Micro)
This C++ script runs on the Arduino Micro. It acts as the direct hardware interface for the Electronic Speed Controllers (ESCs) and motors. It continuously reads the serial buffer for incoming command strings from the Pi, parses the text into integers, and writes the corresponding PWM microsecond signals to the motor pins. It includes a built-in 500ms failsafe that automatically cuts power to all motors (neutralizing to 1500us) if the serial connection is lost or delayed.

