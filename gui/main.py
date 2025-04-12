import tkinter as tk
from tkinter import ttk
import serial
import serial.tools.list_ports
import threading
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

class SerialGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("The trainman")
        self.root.geometry("700x600")

        self.serial_conn = None  # Serial connection object
        self.read_thread = None  # Thread for reading serial data
        self.running = False  # Flag to control the read loop

        ###############################################################################
        # Frame for serial port and baud rate selection
        self.top_frame = ttk.Frame(root)
        self.top_frame.pack(pady=10)

        # Serial Port Selection
        self.port_label = ttk.Label(self.top_frame, text="Serial Port:")
        self.port_label.grid(row=0, column=0, padx=5)

        self.port_combobox = ttk.Combobox(self.top_frame, state="readonly")
        self.port_combobox.grid(row=0, column=1, padx=5)

        # Refresh Button for Serial Ports
        self.refresh_button = ttk.Button(self.top_frame, text="Refresh", command=self.update_ports)
        self.refresh_button.grid(row=0, column=2, padx=5)

        # Baud Rate Selection
        self.baud_label = ttk.Label(self.top_frame, text="Baud Rate:")
        self.baud_label.grid(row=0, column=3, padx=5)

        self.baud_combobox = ttk.Combobox(self.top_frame, state="readonly")
        self.baud_combobox['values'] = ["9600", "115200", "250000"]
        self.baud_combobox.current(1)  # Set default value
        self.baud_combobox.grid(row=0, column=4, padx=5)

        # Connect Button
        self.connect_button = ttk.Button(self.top_frame, text="Connect", command=self.toggle_connection)
        self.connect_button.grid(row=0, column=5, padx=5)

        self.update_ports()

        ###############################################################################

        # Output Text Area
        self.output_text = tk.Text(root, height=20, width=80, state="disabled")
        self.output_text.pack(pady=5)

        # Frame for input field to align it properly
        self.input_frame = ttk.Frame(root)
        self.input_frame.pack(pady=10)

        # Input Entry
        self.input_entry = ttk.Entry(self.input_frame, width=50)
        self.input_entry.grid(row=0, column=0, padx=5)
        self.input_entry.bind("<Return>", lambda event: self.send_text_input_data())

        # Send Button
        self.send_button = ttk.Button(self.input_frame, text="Send", state="disabled", command=self.send_text_input_data)
        self.send_button.grid(row=0, column=1, padx=5)

        ###################################################

        self.slider1_frame = ttk.Frame(root)
        self.slider1_frame.pack(pady=10)

        self.channel1_label = ttk.Label(self.slider1_frame, text="Channel 1:")
        self.channel1_label.grid(row=0, column=0, padx=3)

        self.slider1 = ttk.Scale(self.slider1_frame, from_=500, to=2500, orient="horizontal", command=self.on_slider1_change)
        self.slider1.grid(row=0, column=1, padx=5)

        self.pw1_label = ttk.Label(self.slider1_frame, text="")
        self.pw1_label.grid(row=0, column=2, padx=5)

        self.slider2_frame = ttk.Frame(root)
        self.slider2_frame.pack(pady=10)

        self.channel2_label = ttk.Label(self.slider2_frame, text="Channel 2:")
        self.channel2_label.grid(row=0, column=0, padx=3)

        self.slider2 = ttk.Scale(self.slider2_frame, from_=500, to=2500, orient="horizontal", command=self.on_slider2_change)
        self.slider2.grid(row=0, column=1, padx=5)

        self.pw2_label = ttk.Label(self.slider2_frame, text="")
        self.pw2_label.grid(row=0, column=2, padx=5)

        self.slider3_frame = ttk.Frame(root)
        self.slider3_frame.pack(pady=10)

        self.channel3_label = ttk.Label(self.slider3_frame, text="DAC CH1:")
        self.channel3_label.grid(row=0, column=0, padx=3)

        self.slider3 = ttk.Scale(self.slider3_frame, from_=0, to=4095, orient="horizontal", command=self.on_slider3_change)
        self.slider3.grid(row=0, column=1, padx=5)

        self.pw3_label = ttk.Label(self.slider3_frame, text="")
        self.pw3_label.grid(row=0, column=2, padx=5)

    def on_slider1_change(self, event=None):
        value = int(self.slider1.get())
        self.pw1_label.config(text=f"Pulse Width: {value}")
        self.send_serial_data(f"{{pa{value}}}")

    def on_slider2_change(self, event=None):
        value = int(self.slider2.get())
        self.pw2_label.config(text=f"Pulse Width: {value}")
        self.send_serial_data(f"{{pb{value}}}")

    def on_slider3_change(self, event=None):
        value = int(self.slider3.get())
        self.pw3_label.config(text=f"DAC CH1: {value}")
        self.send_serial_data(f"{{d{value}}}")

    def map_slider_value(self, slider_val):
        return int(750 + (slider_val / 100) * (2200 - 750))

    def send_serial_data(self, data):
        if self.serial_conn:
            self.serial_conn.write((data + "\n").encode("utf-8"))
            self.show_message(f"Sent: {data}")


    def update_ports(self):
        """Update the list of available serial ports."""
        ports = serial.tools.list_ports.comports()
        port_names = [port.device for port in ports]  # Extract only the port names (e.g., COM3, /dev/ttyUSB0)

        self.port_combobox['values'] = port_names  # Update dropdown options
        if port_names:
            self.port_combobox.current(0)  # Select the first port by default
    
    def toggle_connection(self):
        if self.serial_conn is None:
            self.connect_serial()
        else:
            self.disconnect_serial()

    def connect_serial(self):
        port = self.port_combobox.get()
        baudrate = self.baud_combobox.get()

        if not port:
            self.show_message("No serial port selected!")
            return

        try:
            self.serial_conn = serial.Serial(port, baudrate=int(baudrate), timeout=1)
            self.connect_button.config(text="Disconnect")
            self.port_combobox.config(state="disabled")
            self.baud_combobox.config(state="disabled")
            self.send_button.config(state="normal")
            self.show_message(f"Connected to {port} at {baudrate} baud")
            # Start reading data in a separate thread
            self.running = True
            self.read_thread = threading.Thread(target=self.read_serial, daemon=True)
            self.read_thread.start()
        except serial.SerialException as e:
            self.show_message(f"Error: {e}")

    def disconnect_serial(self):
        if self.serial_conn:
            self.running = False  # Stop the read thread
            self.serial_conn.close()
            self.serial_conn = None

        self.connect_button.config(text="Connect")
        self.port_combobox.config(state="readonly")
        self.baud_combobox.config(state="readonly")
        self.send_button.config(state="disabled")
        self.show_message("Disconnected")

    def read_serial(self):
        """Continuously read data from the serial port and display it."""
        while self.running and self.serial_conn:
            try:
                data = self.serial_conn.readline().decode("utf-8").strip()
                if data:
                    self.show_message(f"> {data}")
            except serial.SerialException:
                self.show_message("Error reading from serial port.")
                self.disconnect_serial()
                break

    def send_text_input_data(self):
            data = self.input_entry.get()
            if data:
                self.send_serial_data(data)

    def show_message(self, message):
        self.output_text.config(state="normal")
        self.output_text.insert(tk.END, message + "\n")
        self.output_text.config(state="disabled")
        self.output_text.see(tk.END)  # Auto-scroll to latest message

if __name__ == "__main__":
    root = tk.Tk()
    app = SerialGUI(root)
    root.mainloop()
