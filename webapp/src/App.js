import React, { useState, useEffect } from 'react';
import { Container, Tabs, Tab, Box } from '@mui/material';
import ProgrammingView from './components/ProgrammingView';
import RunMode from './components/RunMode';

const App = () => {
  const [ws, setWs] = useState(null);
  const [currentTab, setCurrentTab] = useState(0);
  const [motorPositions, setMotorPositions] = useState([1225, 1225, 1225, 1225, 1225, 1225]);
  const [availablePrograms] = useState(["Program 1", "Program 2", "Program 3", "Program 4", "Program 5"])

  useEffect(() => {
    const websocket = new WebSocket('ws://127.0.0.1:8000');
    setWs(websocket);

    websocket.onmessage = (event) => {
      console.log('Received from robot:', event.data);
    };

    return () => websocket.close();
  }, []);


  const sendCommand = (rw, mode, cmd, bytes) => {
    if (!ws || ws.readyState !== WebSocket.OPEN) {
      console.log(`Web Socket not connected!`);
      return;
    }

    let byteArray = new Uint8Array(bytes.length+1);
    
    byteArray[0] = ((rw & 1) << 6) | ((mode & 0b111) << 3) | (cmd & 0b111);

    byteArray.set(bytes, 1);

    ws.send(byteArray);
    console.log("ws message:", byteArray);
  };

  const handleSliderChange = (index, newValue) => {
    const newPositions = [...motorPositions];
    newPositions[index] = newValue;
    setMotorPositions(newPositions);
  };

  const sendSignal = () => {
    let byteArray = new Uint8Array(12);

    for (let i = 0; i < 6; i++)
    {
      byteArray[2*i] = motorPositions[i] & 0xff;
      byteArray[2*i+1] = (motorPositions[i] >> 8) & 0xff;
    }
    
    sendCommand(1, 0, 2, byteArray);
  };

  const handleTabChange = (event, newValue) => {
    setCurrentTab(newValue);
  };

  return (
    <Container maxWidth="md" sx={{ padding: 4, textAlign: 'center' }}>
      <Tabs
        value={currentTab}
        onChange={handleTabChange}
        centered
        indicatorColor="primary"
        textColor="primary"
        aria-label="Robot Control Tabs"
      >
        <Tab label="Prog Mode" />
        <Tab label="Run Mode" />
      </Tabs>
      <Box sx={{ marginTop: 4 }}>
        {currentTab === 0 && 
          <ProgrammingView 
            motorPositions={motorPositions}
            handleSliderChange={handleSliderChange}
            sendSignal={sendSignal}
        />}
        {currentTab === 1 && 
        <RunMode 
        availablePrograms={availablePrograms}
        />}
      </Box>
    </Container>
  );
};

export default App;
