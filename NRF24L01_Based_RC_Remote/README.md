# NRF24L01-Based RC Remote

A remote control system built with Arduino Nano and NRF24L01 modules for versatile wireless communication. This system includes a transmitter and receiver setup, making it ideal for remote-controlled devices such as drones, robots, or other IoT applications.

---

## Table of Contents
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Transmitter Wiring](#transmitter-wiring)
- [Receiver Wiring](#receiver-wiring)
- [Setup Instructions](#setup-instructions)
- [Images](#images)
- [How It Works](#how-it-works)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgments](#acknowledgments)

---

## Features
- **Easy to Build**: Uses commonly available components such as Arduino Nano and NRF24L01 modules.
- **Customizable**: Supports additional features like multiple switches and joysticks.
- **Versatile**: Suitable for a wide range of applications, including robotics and IoT projects.
- **Wireless Communication**: Reliable communication using the NRF24L01 module.

---

## Hardware Requirements
### Transmitter:
- **Microcontroller**: Arduino Nano
- **Wireless Module**: NRF24L01
- **Power Supply**: 3.3V (for the NRF24L01 module)

### Receiver:
- **Microcontroller**: Arduino Nano
- **Wireless Module**: NRF24L01
- **Joysticks**: 2 Analog Joysticks
- **Switches**: 4 Push Button Switches

---

## Receiver Wiring

**Microcontroller**: Arduino Nano  

### NRF24L01 Module:
| NRF24L01 Pin | Arduino Nano Pin |
|--------------|------------------|
| **VCC**      | 3.3V            |
| **GND**      | GND             |
| **CE**       | D9              |
| **CSN**      | D10             |
| **SCK**      | D13             |
| **MOSI**     | D11             |
| **MISO**     | D12             |

### Serial Output:
| Component | Connection          |
|-----------|---------------------|
| **USB**   | Computer (for JSON data transmission) |

---

## Transmitter Wiring

**Microcontroller**: Arduino Nano  

### NRF24L01 Module:
| NRF24L01 Pin | Arduino Nano Pin |
|--------------|------------------|
| **VCC**      | 3.3V            |
| **GND**      | GND             |
| **CE**       | D9              |
| **CSN**      | D10             |
| **SCK**      | D13             |
| **MOSI**     | D11             |
| **MISO**     | D12             |

### Joystick Connections:
#### Joystick 1:
| Joystick Pin | Arduino Nano Pin |
|--------------|------------------|
| **VRx**      | A1              |
| **VRy**      | A0              |
| **SW**       | D2              |

#### Joystick 2:
| Joystick Pin | Arduino Nano Pin |
|--------------|------------------|
| **VRx**      | A3              |
| **VRy**      | A2              |
| **SW**       | D3              |

### Switch Connections:
| Switch  | Arduino Nano Pin |
|---------|------------------|
| **SW1** | D4              |
| **SW2** | D5              |
| **SW3** | D6              |
| **SW4** | D7              |

**Note**: All switches' other sides are connected to GND.

---

## Setup Instructions

1. **Hardware Setup**:
   - Follow the wiring diagrams provided above for both the transmitter and receiver.
   - Ensure all connections are secure and correct.

2. **Software Setup**:
   - Install the [RF24 library](https://github.com/nRF24/RF24) in your Arduino IDE.
   - Upload the corresponding transmitter and receiver code to the respective Arduino Nano boards.

3. **Power On**:
   - Power up both the transmitter and receiver.
   - Observe the communication between the devices.

4. **Testing**:
   - Make sure both joysticks and switches are functioning as expected.
   - Verify the JSON data transmission through the USB serial output of the transmitter.

---

## Images

### Transmitter
| Front View | Back View |
|------------|-----------|
| <img src="./Images/Tx_FrontSide.jpeg" alt="Transmitter Front View" width="400"> | <img src="./Images/Tx_Backside.jpeg" alt="Transmitter Back View" width="400"> |

### Receiver
| Front View | Back View |
|------------|-----------|
| <img src="./Images/Rx_FrontSide.jpeg" alt="Receiver Front View" height="500"> | <img src="./Images/Rx_BackSide.jpeg" alt="Receiver Back View" height="500"> |

---

## How It Works
1. The **transmitter** sends joystick and switch data wirelessly using the NRF24L01 module.
2. The **receiver** decodes the data and outputs it for controlling connected devices.
3. Joystick movements are mapped to analog signals while switches provide digital on/off inputs.

---

## Contributing
Contributions are welcome! If you have ideas for improvements or additional features, feel free to:
1. Fork the repository.
2. Create a new branch for your changes.
3. Submit a pull request with a detailed description of your updates.

---

## License
This project is licensed under the MIT License. See the [LICENSE](../LICENSE) file for details.

---

## Acknowledgments
- Special thanks to the creators of the [RF24 library](https://github.com/nRF24/RF24) for facilitating reliable NRF24L01 communication.
- Inspired by various open-source Arduino projects in the IoT community.
