// App.js
import React, { useState, useEffect } from 'react';
import { ThemeProvider } from '@mui/material/styles';
import { Box, Slider, Typography, Container, Button } from '@mui/material';
import theme from './theme';

const App = () => {
  const [motorPositions, setMotorPositions] = useState([0, 0, 0, 0, 0, 0]);
  const [ws, setWs] = useState(null);

  useEffect(() => {
    const websocket = new WebSocket('ws://192.168.4.2.:8000');
    setWs(websocket);

    websocket.onmessage = (event) => {
      console.log('Received from robot:', event.data);
    };

    return () => websocket.close();
  }, []);

  const sendCommand = (command) => {
    if (!ws || ws.readyState !== WebSocket.OPEN) {
      console.log(`Web Socket not connected!`);
      return;
    }

    const byteArray = new Uint8Array(13);

    byteArray[0] = 127;

    for (let i = 0; i < 6; i++)
    {
      byteArray[2 + 2*i] = motorPositions[i] & 0xff;
      byteArray[1 + 2*i] = (motorPositions[i] >> 8) & 0xff;
    }

    ws.send(byteArray);
    console.log("ws message:", byteArray);
  };

  // Function to handle slider changes
  const handleSliderChange = (index) => (event, newValue) => {
    const newPositions = [...motorPositions];
    newPositions[index] = newValue;
    setMotorPositions(newPositions);
    
  };

  return (
    <ThemeProvider theme={theme}>
      <Container
        maxWidth="md"
        sx={{
          textAlign: 'center',
          color: 'text.primary',
          padding: 4,
          backgroundColor: 'background.default',
        }}
      >
        <Typography variant="h4" color="primary" gutterBottom>
          Robot Arm Control Panel
        </Typography>

        <Box display="flex" justifyContent="center" gap={3} marginBottom={4}>
          {motorPositions.map((position, index) => (
            <Box key={index} display="flex" flexDirection="column" alignItems="center">
              <Typography variant="h6" color="primary">
                Motor {index + 1}
              </Typography>
              <Slider
                orientation="vertical"
                value={position}
                onChange={handleSliderChange(index)}
                min={410}
                max={2048}
                sx={{ height: 200, color: 'primary.main' }}
              />
              <Typography variant="body2" color="text.primary">
                {position}
              </Typography>
            </Box>  
          ))}
        </Box>

        <Button
            variant="contained"           // Styling the button
            color="primary"               // Choose color scheme
            onClick={sendCommand}         // Define onClick event
            sx={{ padding: '10px 20px' }} // Optional: MUI styling
            >
            Set Positions
          </Button>
      </Container>
    </ThemeProvider>
  );
};

export default App;
