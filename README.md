# pico-microros

Runs a micro-ROS node on a Raspberry Pi Pico W, publishing an incrementing counter to a ROS 2 network over WiFi via UDP transport.

This serves as a minimal working example of micro-ROS on the Pico W — useful as a starting point before adding sensor logic.

---

## Hardware

- Raspberry Pi Pico W (or Pico 2W)
- Host machine running ROS 2 Jazzy on the same WiFi network

---

## Prerequisites

Install dependencies:

```bash
sudo apt install build-essential cmake gcc-arm-none-eabi libnewlib-arm-none-eabi doxygen git python3
```

Install the Pico SDK:

```bash
cd ~
git clone --recurse-submodules https://github.com/raspberrypi/pico-sdk.git
echo "export PICO_SDK_PATH=$HOME/pico-sdk" >> ~/.bashrc
source ~/.bashrc
```

Install the GCC ARM toolchain (9.3.1):

```bash
# Download from https://developer.arm.com/downloads/-/gnu-rm
# Extract and move to home directory, then:
echo "export PICO_TOOLCHAIN_PATH=$HOME/gcc-arm-none-eabi-9-2020-q2-update/" >> ~/.bashrc
source ~/.bashrc
```

---

## Configuration

Before building, edit the following in `pico_micro_ros_example.c`:

```c
char ssid[] = "SSID";      // your WiFi network name
char pass[] = "PASSWORD";  // your WiFi password
```

And set your host machine's IP address in `picow_udp_transports.h`.

If using a **Pico 2W**, update line 2 of `CMakeLists.txt` accordingly.

---

## Build

```bash
cd ~/micro_ros_ws/src/pico-microros/
mkdir build && cd build
cmake ..
make
```

---

## Flash

Hold **BOOTSEL** on the Pico while plugging it in via USB, then:

**Pico W:**
```bash
cp build/pico_micro_ros_example.uf2 /media/$USER/RPI-RP2
```

**Pico 2W:**
```bash
cp build/pico_micro_ros_example.uf2 /media/$USER/RP2350
```

---

## Run the micro-ROS Agent

Disconnect the Pico from USB and power it externally, then run the agent on your host machine:

```bash
sudo docker run -it --rm --net=host microros/micro-ros-agent:jazzy udp4 -p 8888
```

The Pico searches for the agent for 60 seconds after startup.

Verify the topic is live:

```bash
ros2 topic list
ros2 topic echo /pico_counter
```

---

## Dependencies

- [micro-ROS Raspberry Pi Pico SDK](https://github.com/micro-ROS/micro_ros_raspberrypi_pico_sdk)
- [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk)
- ROS 2 Jazzy
