import serial
import csv
import time

serial_port = '/dev/tty.usbmodem101'
baud_rate = 9600  
output_file_path = '/Users/linxiran/Desktop/output.csv'

try:
    # Initialize serial port
    ser = serial.Serial(serial_port, baud_rate, timeout=1)
    print(f"Connected to {serial_port}")

    # Open CSV file to write
    with open(output_file_path, mode='w', newline='') as file:  # Fixed variable name
        writer = csv.writer(file)
        # writer.writerow(['Column1', 'Column2', 'Column3']) 

        while True:
            # Read data from serial port
            if ser.in_waiting > 0:
                data = ser.readline().decode('utf-8').rstrip()
                print(data)
                # Assuming data is comma-separated, write directly to CSV
                writer.writerow(data.split(','))  

except serial.SerialException as e:
    print(f"Unable to open serial port {serial_port}: {e}")

except KeyboardInterrupt:
    print("\nProgram terminated by user")

finally:
    ser.close()
    print("Serial port closed")

