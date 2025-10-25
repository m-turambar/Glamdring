import unittest
import serial
import time
import random
import string


class EchoTest(unittest.TestCase):
    PORT = "COM5"
    BAUDRATE = 115200

    @classmethod
    def setUpClass(cls):
        """Runs once before all tests"""
        cls.ser = serial.Serial(cls.PORT, cls.BAUDRATE, timeout=2)
        time.sleep(2)  # give MCU time to reset
        cls.ser.reset_input_buffer()
        cls.ser.reset_output_buffer()

    @classmethod
    def tearDownClass(cls):
        """Runs once after all tests"""
        cls.ser.close()

    def send_and_receive(self, message: bytes) -> bytes:
        """Helper: write and read back"""
        self.ser.reset_input_buffer()
        self.ser.reset_output_buffer()
        self.ser.write(message)
        return self.ser.read(len(message))

    def test_short_message(self):
        msg = b"Hello\n"
        resp = self.send_and_receive(msg)
        self.assertEqual(resp, msg)

    def test_long_message(self):
        msg = b"A" * 256 + b"\n"
        resp = self.send_and_receive(msg)
        self.assertEqual(resp, msg)

    def test_random_ascii(self):
        msg = ''.join(random.choice(string.printable) for _ in range(64)).encode()
        resp = self.send_and_receive(msg)
        self.assertEqual(resp, msg)

    def test_binary_data(self):
        # Random bytes 0–255
        msg = bytes(random.getrandbits(8) for _ in range(64))
        resp = self.send_and_receive(msg)
        self.assertEqual(resp, msg)

    def test_empty_message(self):
        msg = b""
        resp = self.send_and_receive(msg)
        self.assertEqual(resp, msg)


if __name__ == "__main__":
    unittest.main()
