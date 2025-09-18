import serial
import time

# prueba generada con GPT.

def test_echo(port="COM5", baudrate=115200, message=b"Hola Micro!\n"):
    try:
        # Open the serial port
        with serial.Serial(port, baudrate, timeout=1) as ser:
            time.sleep(2)  # Give MCU time to reset if needed

            # Clear input/output buffers
            ser.reset_input_buffer()
            ser.reset_output_buffer()

            # Send data
            print(f"Sending: {message!r}")
            ser.write(message)

            # Read back the same amount
            response = ser.read(len(message))

            print(f"Received: {response!r}")

            # Check if echoed correctly
            if response == message:
                print("✅ Echo test passed")
                return True
            else:
                print("❌ Echo test failed")
                return False

    except serial.SerialException as e:
        print(f"Serial error: {e}")
        return False


if __name__ == "__main__":
    test_echo()
