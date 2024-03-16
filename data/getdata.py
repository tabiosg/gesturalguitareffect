import serial
import subprocess

# Configure the serial port
ser = serial.Serial('/dev/ttyACM0', 9600)  # Replace 'COM1' with your COM port

# Open the file for writing
file_name = "alldata.txt"
with open(file_name, 'w') as file:
    try:
        while True:
            # Read data from the serial port
            data = ser.readline().decode().strip()  # Decode bytes to string and strip newline characters
            
            # Write data to the file
            file.write(data + '\n')
            file.flush()  # Ensure data is written to the file immediately
            print("Received:", data)  # Print received data
            
    except KeyboardInterrupt:
        print("Keyboard Interrupt, closing the port.")
        ser.close()
        print("Running graph.py...")
        subprocess.run(["python", "graph.py"])  # Replace "python" with your Python interpreter if needed
