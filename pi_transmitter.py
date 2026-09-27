import time
import struct
from pyrf24 import RF24, RF24_PA_LOW

# Initialize radio
radio = RF24(22, 0)
if not radio.begin():
    raise RuntimeError("Radio hardware not responding!")

radio.pa_level = RF24_PA_LOW
address = b"1Node"

radio.openWritingPipe(address)
radio.listen = False

def send_commands(m1, m2=1500, m3=1500, m4=1500):
    """Packs 4 motor PWM values into an 8-byte payload and sends it."""
    # Pack into little-endian unsigned shorts
    payload = struct.pack('<4H', m1, m2, m3, m4)
    
    # Pad the remaining bytes to hit the 32-byte NRF24 default payload size
    buffer = payload.ljust(32, b'\x00')
    
    success = radio.write(buffer)
    if not success:
        print("Dropped packet - No ACK received")

print("Starting motor test transmission sequence...")

try:
    while True:
        print("1. SPIN UP")
        for speed in range(1500, 1705, 5):
            send_commands(speed, 1500, 1500, 1500)
            time.sleep(0.05) # Updating at 20Hz

        print("2. HOLD")
        for _ in range(40): # Hold for 2 seconds (40 * 0.05s)
            send_commands(1700, 1500, 1500, 1500)
            time.sleep(0.05)

        print("3. SPIN DOWN")
        for speed in range(1700, 1495, -5):
            send_commands(speed, 1500, 1500, 1500)
            time.sleep(0.05)

        print("4. REST")
        for _ in range(60): # Rest for 3 seconds
            send_commands(1500, 1500, 1500, 1500)
            time.sleep(0.05)

except KeyboardInterrupt:
    # Safely shut down motors if you Ctrl+C the script
    print("\nScript interrupted. Sending neutral commands...")
    for _ in range(10):
        send_commands(1500, 1500, 1500, 1500)
        time.sleep(0.05)
