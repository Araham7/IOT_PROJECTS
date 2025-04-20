import socket
import wave

# Server details
UDP_IP = "0.0.0.0"  # Bind to all available network interfaces
UDP_PORT = 12345     # Same port as in ESP32 code

# Create the UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Bind the socket to the IP and port
sock.bind((UDP_IP, UDP_PORT))

print(f"Listening on {UDP_IP}:{UDP_PORT} for incoming data...")

# WAV parameters
SAMPLE_RATE = 16000  # Same as in ESP32 code
SAMPLE_WIDTH = 2     # 16-bit samples (2 bytes per sample)
NUM_CHANNELS = 1     # Mono audio
BUFFER_SIZE = 1024   # Adjust this depending on the expected data size

# Create a wave file to save the received audio
with wave.open("received_audio.wav", "wb") as wave_file:
    wave_file.setnchannels(NUM_CHANNELS)  # Mono audio
    wave_file.setsampwidth(SAMPLE_WIDTH)  # 2 bytes per sample
    wave_file.setframerate(SAMPLE_RATE)   # 16000 Hz sample rate

    while True:
        # Receive data from the ESP32 (UDP packets)
        data, addr = sock.recvfrom(BUFFER_SIZE)  # Receive UDP packet
        
        # Debugging output
        if data:
            print(f"Received {len(data)} bytes from {addr}")
        else:
            print("No data received")

        if data:
            # Write the received data to the WAV file
            wave_file.writeframes(data)
            print(f"Writing {len(data)} bytes to the WAV file")



