import serial, time, requests, psutil
import serial.tools.list_ports

TARGET_NAME = "Leonardo"
BAUD_RATE = 9600
DELAY_BETWEEN_TRIES = 1

def get_sensor_value(data, name):
    for item in data:
        if item.get("SensorName") == name:
            try:
                return float(item.get("SensorValue", 0))
            except:
                return 0.0
    return 0.0

def find_device():
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if TARGET_NAME in port.description:
            return port.device
    return None

def connect():
    while True:
        port = find_device()
        if port:
            try:
                ser = serial.Serial(port, BAUD_RATE)
                print(f"✅ Connected to {port}")
                time.sleep(2)  # Allow Arduino to reset
                return ser
            except Exception as e:
                print(f"⚠️  Failed to open {port}: {e}")
        print("🔍 Waiting for device...")
        time.sleep(DELAY_BETWEEN_TRIES)

ser = connect()

while True:
    try:
        resp = requests.get("http://localhost:6969/", timeout=2)
        raw_text = resp.text.strip()

        if not raw_text.startswith("["):
            print("⚠️ Invalid response:\n", raw_text)
            time.sleep(1)
            continue

        data = resp.json()

        cpu_usage = int(get_sensor_value(data, "Total CPU Usage"))
        cpu_temp = int(get_sensor_value(data, "P-core 0"))
        gpu_temp = int(get_sensor_value(data, "GPU Temperature"))
        gpu_usage = int(get_sensor_value(data, "GPU Utilization"))

        ram = psutil.virtual_memory()
        ram_used = int(ram.used / (1024 ** 3))
        ram_avail = int(ram.available / (1024 ** 3))

        tx = int(get_sensor_value(data, "Current UP rate"))
        rx = int(get_sensor_value(data, "Current DL rate"))

        output = (
            f"CPU:{cpu_usage:>3}%  TEMP:{cpu_temp} C\n"
            f"GPU:{gpu_usage:>3}%  TEMP:{gpu_temp} C\n"
            f"RAM:{ram_used} GB  AVL:{ram_avail} GB\n"
            f"TX:{tx}KB/s  RX:{rx}KB/s"
        )

        print("Sending:\n", output, "\n")
        ser.write((output + "\n").encode())

    except serial.SerialException as e:
        print("🔌 Serial disconnected. Reconnecting...")
        ser.close()
        ser = connect()

    except KeyboardInterrupt:
        print("🛑 Exiting by user.")
        ser.close()
        break

    except Exception as e:
        print("💥 Error:", e)

    time.sleep(1)
