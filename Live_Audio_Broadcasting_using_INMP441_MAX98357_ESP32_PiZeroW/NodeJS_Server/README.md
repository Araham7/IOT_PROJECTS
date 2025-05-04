# WebSocket Audio Streaming Server

This project implements a Node.js-based WebSocket server that facilitates real-time audio streaming between an ESP32 transmitter and one or more ESP32 receivers. The server listens on port 8080, accepts binary audio data from a single transmitter (e.g., an ESP32 with an INMP441 microphone), and forwards it to all connected receivers (e.g., ESP32s with MAX98357A amplifiers and speakers).

## Project Overview

The WebSocket server is designed to:

* Accept a single transmitter client, which sends 16-bit, 16kHz audio data.

* Support multiple receiver clients, which receive the audio data for playback.

* Handle binary data to ensure efficient audio streaming.

* Log connection status, data transfer, and errors for debugging.

This server is part of an audio streaming system that includes:

* **ESP32 Audio Transmitter**: Captures audio via an I2S microphone and sends it to this server.

* **ESP32 Audio Receiver**: Receives audio from this server and plays it through a speaker.

## Prerequisites

* **Node.js** (v14 or later recommended)

* **npm** (Node package manager)

* A local network with the server and ESP32 devices connected (e.g., WiFi network named "Laptop").

* ESP32 transmitter and receiver devices configured with the appropriate IP address of the server (default: 192.168.239.231).

## Installation

1. **Clone or Create the Project**: Create a new directory for the project and save the server code as `server.js`.

2. **Initialize Node.js Project**: Run the following command in the project directory to create a **package.json** file:

```bash
npm init -y
```

3. **Install Dependencies**: Install the `ws` WebSocket library:

```bash
npm install ws
```

4. **Save the Server Code**: Ensure the following code is saved as server.js in the project directory:

```javascript
const WebSocket = require('ws');

const wss = new WebSocket.Server({ port: 8080 });
let transmitter = null;
let receivers = new Set();

wss.on('connection', (ws) => {
  console.log('New client connected');

  ws.on('message', (data, isBinary) => {
    if (isBinary && transmitter === ws) {
      console.log(`Received ${data.length} bytes from transmitter`);
      let forwarded = 0;
      receivers.forEach((receiver) => {
        if (receiver.readyState === WebSocket.OPEN) {
          receiver.send(data, { binary: true });
          forwarded++;
        }
      });
      console.log(`Forwarded to ${forwarded} receivers`);
    }
  });

  ws.on('close', () => {
    if (ws === transmitter) {
      console.log('Transmitter disconnected');
      transmitter = null;
    } else {
      console.log('Receiver disconnected');
      receivers.delete(ws);
    }
  });

  // Assign role: first client is transmitter, others are receivers
  if (!transmitter) {
    transmitter = ws;
    console.log('Client assigned as transmitter');
  } else {
    receivers.add(ws);
    console.log('Client assigned as receiver');
  }
});

wss.on('listening', () => {
  console.log('WebSocket server listening on port 8080');
});

wss.on('error', (err) => {
  console.error('WebSocket server error:', err);
});
```

5. **Run the Server:** Start the server with:

```bash
node server.js
```

The server will listen on port 8080 and log `WebSocket server listening on port 8080`.


# Usage

1. **Configure ESP32 Devices:**

* Ensure the ESP32 transmitter and receiver sketches are configured with the correct WiFi credentials (`ssid: "Your_hotspot_ssid"`, password: "Your_hotspot_password") and server IP (ws_host: "Your_computer_ip_where_server_is_running", ws_port: 8080).

* Upload the transmitter and receiver sketches to their respective ESP32 boards.

2. Start the System:

* Run the WebSocket server (node server.js).
* Power on the ESP32 transmitter (with an I2S microphone, e.g., INMP441).
* Power on one or more ESP32 receivers (with MAX98357A amplifiers and speakers).
* The transmitter connects first and is assigned the transmitter role. Subsequent connections are assigned as receivers.

4. Stream Audio:

* The transmitter captures audio and sends it to the server.
* The server forwards the audio data to all connected receivers, which play it through their speakers.
* Monitor the server’s console output for connection status and data transfer logs.

## Debugging:

* Check the server logs for messages like "New client connected", "Received X bytes from transmitter", or "Forwarded to Y receivers".

* If no audio is streamed, verify the ESP32 devices are connected to the WiFi network and the server IP is correct.

* Ensure port 8080 is not blocked by a firewall.

## Project Structure
```
project-directory/
├── server.js          # WebSocket server code
├── package.json       # Node.js project 
└── node_modules/      # Installed dependencies (ws)
```

`Notes:---`

* **Network Configuration:**

  * The server IP (e.g, 192.168.239.231) must match the IP of the machine running the server. Update the ESP32 sketches if the server runs on a different IP.
  * Ensure all devices are on the same local network.

* **Scalability:**

  * The server supports one transmitter and multiple receivers. If the transmitter disconnects, the next connecting client becomes the transmitter.

  * For high-latency networks, consider increasing the WebSocket reconnect interval in the ESP32 sketches.

* **Performance:**

  * The server handles binary data efficiently for 16-bit, 16kHz audio.

  * For large numbers of receivers, monitor server performance and network bandwidth.

* **Security:**

  * This is a basic WebSocket server without authentication or encryption. For production use, consider adding WebSocket Secure (wss://) and client authentication.

## Troubleshooting

* **Server Fails to Start:**

* Ensure port 8080 is free (`netstat -an | grep 8080` or equivalent).

* Verify Node.js and the `ws` package are installed correctly.

## ESP32 Clients Don’t Connect:

* Check the server IP and port in the ESP32 sketches.

* Ensure the WiFi network is accessible and credentials are correct.

* Look for "WebSocket Disconnected" or "WebSocket Error" in the ESP32 Serial Monitor.

## No Audio Received:

* Confirm the transmitter is sending data (check server logs for "Received X bytes").

* Verify receivers are connected (server logs show "Client assigned as receiver").

* Check ESP32 wiring and microphone/speaker functionality.

## Related Projects

This server is designed to work with:

* **ESP32 Audio Transmitter:** Captures audio via an I2S microphone and sends it to this server.

* **ESP32 Audio Receiver:** Receives audio from this server and plays it through a MAX98357A amplifier and speaker.

Ensure both ESP32 projects are configured to connect to this server’s IP and port.