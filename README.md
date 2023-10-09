# ESP32 WiFi UART Serial Bridge

This is a UART <-> WiFi serial bridge, orignially created to provide a WiFi
serial port for my 3D printer using an ESP32. It also optionally serves a 
second TCP server for triggering a GPIO pin, for example to trigger a PSU relay.

## Building
1. Open up project with PlatformIO inside VSCode.
2. Edit upload settings in `platformio.ini`.
3. Configure WiFi settings in `include/config.h`, and modify settings as needed.
4. Click "upload" in the PlatformIO project tasks.

## Using remote serial port
Example scenario: your ESP32 IP is 10.10.0.1, port 23.

Every byte sent to the TCP server `10.10.0.1:23` will be passed to the configured
UART port. One client can be connected at a time. This is compatible with the 
`pyserial` library's `socket://` protocol, as well as the Octoprint 
[network printing plugin](https://github.com/hellerbarde/OctoPrint-Network-Printing).

## Using PSU relay service
Example scenario: your ESP32 IP is 10.10.0.1, relay service running on port 5050.

The PSU relay TCP service reads the first byte as a command from a connected 
client and then disconnects that client.

**Commands**

- `'1'` or `0x01`: turn on PSU (set configured pin HIGH)
- `'0'` or `0x00`: turn off PSU (set configured pin LOW)
-  `'S'`: return pin status (single char `'1'` or `'0'` returned)

**One-shot terminal commands**

- turn on: `echo "1" | nc 10.10.0.1 5050`
- turn off: `echo "0" | nc 10.10.0.1 5050`