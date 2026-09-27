import time
import serial
import struct
from pyrf24 import RF24, RF24_PA_LOW

# Connect to the Arduino via USB Serial
try:
    arduino = serial.Serial('/dev/ttyACM0', 115200, timeout=0.1)
    time.sleep(2) # Wait for Arduino to reset after serial connection
except Exception as e:
    raise RuntimeError(f"Could not connect to Arduino. Check port name. Error: {e}")

# Initialize radio on CE=22, CSN=0
radio = RF24(22, 0)
if not radio.begin():
    raise RuntimeError("Radio hardware not responding!")

radio.pa_level = RF24_PA_LOW
address = b"1Node"

radio.openReadingPipe(1, address)
radio.listen = True

print("Listening for incoming motor commands...")

while True:
    if radio.available():
        # Read the payload
        payload = radio.read(radio.payload_size)
        
        try:
            # Unpack 4 unsigned shorts (16-bit ints) from the first 8 bytes. 
            # '<4H' = Little-endian, 4x Unsigned Shorts
            motor_commands = struct.unpack('<4H', payload[:8])
            
            # Format as comma-separated string with a newline terminator
            serial_msg = f"{motor_commands[0]},{motor_commands[1]},{motor_commands[2]},{motor_commands[3]}\n"
            
            # Forward to Arduino
            arduino.write(serial_msg.encode('utf-8'))
            print(f"Relayed: {serial_msg.strip()}")
            
        except struct.error:
            print("Received malformed packet.")
            
    # Short sleep to prevent maxing out the CPU. 
    # 0.005s supports up to 200Hz update rates, plenty for PID.
    time.sleep(0.005)
