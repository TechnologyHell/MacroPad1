import requests

url = "http://localhost:6969/json.json"
data = requests.get(url).json()

# Initialize all values
cpu_temp = cpu_usage = gpu_temp = gpu_usage = tx = rx = 0
ram_used = ram_avail = 0

for sensor in data.get("Sensors", []):
    label = sensor.get("Name", "")
    try:
        value = float(sensor.get("Value", 0))
    except:
        value = 0

    if label == "CPU Die (average)":
        cpu_temp = value
    elif label == "Total CPU Usage":
        cpu_usage = value
    elif label == "GPU Temperature":
        gpu_temp = value
    elif label == "GPU D3D Usage":
        gpu_usage = value
    elif label == "Physical Memory Used":
        ram_used = value
    elif label == "Physical Memory Available":
        ram_avail = value
    elif label == "Current UP rate":
        tx = value
    elif label == "Current DL rate":
        rx = value

# Calculate RAM percent safely
ram_total = ram_used + ram_avail
ram_percent = (ram_used / ram_total) * 100 if ram_total else 0

# Now print safely
print(f"CPU : {cpu_usage:.0f}%     TEMP : {cpu_temp:.0f}°C")
print(f"GPU : {gpu_usage:.0f}%     TEMP : {gpu_temp:.0f}°C")
print(f"RAM : {ram_percent:.1f}%")
print(f"TX  : {tx:.0f} KB/s  RX : {rx:.0f} KB/s")
