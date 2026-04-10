# pico-microros

Runs a micro-ROS node on a Raspberry Pi Pico W, publishing an incrementing counter to a ROS 2 network over WiFi via UDP transport.

This serves as a minimal working example of micro-ROS on the Pico W — useful as a starting point before adding sensor logic.

---

## Hardware

- Raspberry Pi Pico W or Pico 2W
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

**Board selection:** This repo defaults to the Pico 2W. If you're using the original Pico W, change line 2 of `CMakeLists.txt`:

```cmake
set(PICO_BOARD pico_w)    # original Pico W
# set(PICO_BOARD pico2_w) # Pico 2W
```

**WiFi credentials:** Edit `pico_micro_ros_example.c`:

```c
char ssid[] = "SSID";      // your WiFi network name
char pass[] = "PASSWORD";  // your WiFi password
```

**Agent IP address:** Set your host machine's IP in `picow_udp_transports.h`.

---

## Build the libmicroros Static Library

`libmicroros` is a precompiled static library required to build this project. It is not included in the repo and must be generated once using Docker.

Clone the micro-ROS Pico SDK and run the builder:

```bash
cd ~/micro_ros_ws/src
git clone https://github.com/micro-ROS/micro_ros_raspberrypi_pico_sdk.git
cd micro_ros_raspberrypi_pico_sdk
docker run --rm -v $(pwd):/project --env PICO_SDK_PATH=/project/pico-sdk microros/micro_ros_static_library_builder:jazzy
```

Then copy the output into this project:

```bash
cp -r ~/micro_ros_ws/src/micro_ros_raspberrypi_pico_sdk/libmicroros \
      ~/micro_ros_ws/src/pico-microros/libmicroros
```

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
