import React, { useState, useEffect, useCallback } from 'react';
import { Container, Tabs, Tab, Box } from '@mui/material';
import ProgrammingView from './components/ProgrammingView';
import RunMode from './components/RunMode';

const MSG_ERROR = 0
const MSG_OKAY = 1
const MSG_READ = 0
const MSG_WRITE = 1

const STATE_IDLE = 0
const STATE_PROGRAM = 1
const STATE_RUNNING = 2

const CMD_WAYPOINT_COUNT = 0
const CMD_WAYPOINT_IDX = 1
const CMD_WAYPOINT_CUR = 2
const CMD_PROGRAM_COUNT = 3
const CMD_PROGRAM_NAME = 4
const CMD_PROGRAM_RW = 5
const CMD_RUN = 6

const SERVER_IP = "127.0.0.1"
const SERVER_PORT = 8000

const robotSendCommand = (websocket, rw, mode, cmd, bytes) => {
  if (!websocket || websocket.readyState !== WebSocket.OPEN) {
    console.log(`Web Socket not connected!`);
    return;
  }

  let length = 1

  if (bytes !== null)
  {
    length += bytes.length;
  }
  
  let byteArray = new Uint8Array(length);
  byteArray[0] = ((rw & 1) << 6) | ((mode & 0b111) << 3) | (cmd & 0b111);
  
  if (bytes != null)
  {
    byteArray.set(bytes, 1);
  }
  
  websocket.send(byteArray);
  console.log("ws message:", byteArray);
};

const App = () => {
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Vars
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  const [websocket, setWebsocket] = useState(null);
  const [currentTab, setCurrentTab] = useState(0);
  const [waypointIndex, setWaypointIndex] = useState(0);
  const [waypointCount, setWaypointCount] = useState(100);
  const [waypoint, setWaypoint] = useState([1225, 1225, 1225, 1225, 1225, 1225]);
  const [waypointTarget, setWaypointTarget] = useState([1225, 1225, 1225, 1225, 1225, 1225]);
  const [motorPositions, setMotorPositions] = useState([1225, 1225, 1225, 1225, 1225, 1225]);
  const [programCount, setProgramCount] = useState(0);
  const [programNames, setProgramNames] = useState(["Program"]);


  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // UI Related Functions
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////

  const handleSliderChange = (index, newValue) => {
    const newPositions = [...waypoint];
    newPositions[index] = newValue;
    setWaypoint(newPositions);
  };

  const handleTabChange = (event, newValue) => {
    setCurrentTab(newValue);
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Robot Comms
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////

  const robotProcessMessage = (msg) => {
    let err = !((msg[0] >> 7) & 1);
    let rw = (msg[0] >> 6) & 1;
    let state = (msg[0] >> 3) & 0b111;
    let cmd = msg[0] & 0b111;
    let data = msg.slice(1);
    console.log(`err: ${err}, cmd: ${cmd}, data: ${data}`)
    switch (cmd) {
      case CMD_WAYPOINT_COUNT:
        setWaypointCount(data[0]);
        console.log(`waypoint count: ${waypointCount}`);
        break;
      case CMD_WAYPOINT_IDX:
        setWaypointIndex(data[0]);
        console.log(`waypoint index: ${waypointIndex}`);
        break;
      case CMD_WAYPOINT_CUR:
        // ...
        break;
      case CMD_PROGRAM_COUNT:
        setProgramCount(data[0]);
        console.log(`program count: ${programCount}`);
        break;
      case CMD_PROGRAM_NAME:
        const updatedNames = [...programNames];
        updatedNames[data[0]] = String.fromCharCode(...data.slice(1));
        setProgramNames(updatedNames);
      break;
      case CMD_PROGRAM_RW:
        // ...
        break;
      case CMD_RUN:
        // ...
        break;
      default:
        break;
    }
  };

  const robotGetProgramNames = () => {
    for (let i = 0; i < 5; i++)
    {
      robotSendCommand(websocket, MSG_READ, STATE_IDLE, CMD_PROGRAM_NAME, new Uint8Array([i]));
    }
  }

  const robotSetWaypointIndex = () => {
    // to do
  }

  const robotSetWaypoint = () => {
    let byteArray = new Uint8Array(12);

    for (let i = 0; i < 6; i++)
    {
      byteArray[2*i] = waypoint[i] & 0xff;
      byteArray[2*i+1] = (waypoint[i] >> 8) & 0xff;
    }

    robotSendCommand(websocket, 1, 0, 2, byteArray);
  };

  const robotSaveProgram = (programIndex, programName) => {
    const encoder = new TextEncoder();
    const stringBytes = encoder.encode(programName);
    const byteArray = new Uint8Array(1 + stringBytes.length);
    const progIdx = new  Uint8Array([programIndex]);

    byteArray.set(progIdx, 0);
    byteArray.set(stringBytes, 1);

    robotSendCommand(websocket, MSG_WRITE, 0, CMD_PROGRAM_NAME, byteArray);
    robotSendCommand(websocket, MSG_WRITE, 0, CMD_PROGRAM_RW, progIdx);
  }

  const robotRunProgram = (programIndex) => {
    robotSendCommand(websocket, MSG_READ, 0, CMD_PROGRAM_RW, new Uint8Array([programIndex]));
    robotSendCommand(websocket, MSG_WRITE, 0, CMD_RUN, new Uint8Array([1]));
  }

  const robotStopProgram = () => {
    robotSendCommand(websocket, MSG_WRITE, 0, CMD_RUN, new Uint8Array([0]));
  }

  useEffect(() => {
    const websocket = new WebSocket(`ws://${SERVER_IP}:${SERVER_PORT}`);
    websocket.binaryType = "arraybuffer";

    setWebsocket(websocket);

    websocket.onopen = () => {
      console.log("WebSocket connected");
      robotSendCommand(websocket, MSG_READ, STATE_IDLE, CMD_WAYPOINT_COUNT, null);
      robotSendCommand(websocket, MSG_READ, STATE_IDLE, CMD_WAYPOINT_IDX, null);
      robotSendCommand(websocket, MSG_READ, STATE_IDLE, CMD_WAYPOINT_CUR, null);
      robotSendCommand(websocket, MSG_READ, STATE_IDLE, CMD_PROGRAM_COUNT, null);
      for (let i = 0; i < 5; i++)
      {
        robotSendCommand(websocket, MSG_READ, STATE_IDLE, CMD_PROGRAM_NAME, new Uint8Array([i]));
      }
    };

    websocket.onmessage = (event) => {
      console.log('Received from robot:', event.data);
      const arrayBuffer = event.data;
      const byteArray = new Uint8Array(arrayBuffer);
      robotProcessMessage(byteArray);
    };

    websocket.onerror = (error) => {
      console.error("WebSocket error:", error);
    };
  
    websocket.onclose = () => {
      console.log("WebSocket closed");
    };

    return () => websocket.close();
  }, []);

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Render Section
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
            waypoint={waypoint}
            handleSliderChange={handleSliderChange}
            sendWaypoint={robotSetWaypoint}
            saveProgram={robotSaveProgram}
        />}
        {currentTab === 1 && 
        <RunMode 
          availablePrograms={programNames}
          motorPositions={motorPositions}
          targetWaypoint={waypointTarget}
          startProgram={robotRunProgram}
          stopProgram={robotStopProgram}
        />}
      </Box>
    </Container>
  );
};

export default App;