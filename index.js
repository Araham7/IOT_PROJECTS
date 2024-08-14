const { SerialPort } = require('serialport');

const port = new SerialPort({
  path: '/dev/ttyUSB0',
  baudRate: 9600,
  autoOpen: false,
});

let buffer = ''; // Buffer to accumulate data chunks

port.open(function (err) {
  if (err) {
    return console.log('Error opening port: ', err.message);
  }

  console.log('Port opened successfully');
  
  // Write to the serial port if needed
  port.write('main screen turn on');
});

// The open event is always emitted
port.on('open', function() {
  console.log('Serial port is open');
  
  // Read data from the serial port
  port.on('data', function(data) {
    buffer += data.toString(); // Append received data to buffer

    // Check for a complete message (assuming '\n' as the delimiter)
    let delimiterIndex;
    while ((delimiterIndex = buffer.indexOf('\n')) !== -1) {
      const completeMessage = buffer.slice(0, delimiterIndex + 1); // Extract the complete message
      buffer = buffer.slice(delimiterIndex + 1); // Remove the processed message from the buffer

      console.log('Complete data received:', completeMessage.trim()); // Process the complete message
    }
  });
});

port.on('error', function(err) {
  console.log('Error: ', err.message);
});
