/*
The contents of this code and accompanying instructions are the intellectual property of Araham Abeddin. 
They are licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International Public License. 

This license allows you to remix, adapt, and build upon the material for non-commercial purposes, 
provided that appropriate credit is given to Araham Abeddin. You must not suggest that Araham Abeddin endorses you or your use of the work. 
Any derivative works must be distributed under the same license.

This code and documentation are provided "as is" without any warranties of any kind. 
Neither Araham Abeddin nor the author assumes any liability for any loss or damage caused, 
or claimed to be caused, directly or indirectly by the instructions, software, or hardware described herein.

As Araham Abeddin has no control over the use, setup, assembly, modification, or misuse of the hardware, 
software, or information provided in this manual, no responsibility shall be accepted for any resulting injury or damage. 
By copying, using, setting up, or assembling this material, the user accepts full liability for any consequences.

Version 1.0 – Released on 4 May 2025
*/





// NodeJs Based Websocket Server :---

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