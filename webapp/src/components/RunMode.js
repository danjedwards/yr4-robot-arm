import React, { useState } from 'react';
import { Box, Button, Select, MenuItem, Typography, FormControl, InputLabel, Table, TableBody, TableCell, TableContainer, TableHead, TableRow, Paper } from '@mui/material';
import motorNames from '../globals';

const RunMode = ({ availablePrograms }) => {
  const [selectedProgram, setSelectedProgram] = useState('');
  const [isRunning, setIsRunning] = useState(false);
  const [currentWaypoint, setCurrentWaypoint] = useState(0);
  const [motorPositions, setMotorPositions] = useState(
    Object.values(motorNames).map(() => ({ current: 1200, target: 1200 }))
  );

  // Handle program selection
  const handleProgramChange = (event) => {
    setSelectedProgram(event.target.value);
  };

  // Toggle Start/Stop button
  const handleToggleRun = () => {
    setIsRunning((prev) => !prev);
    if (!isRunning) {
      startProgram();
    } else {
      stopProgram();
    }
  };

  // Dummy functions for starting/stopping program and updating positions
  const startProgram = () => {
    console.log(`Starting program: ${selectedProgram}`);
    // Logic to start the program and update motor positions goes here
  };

  const stopProgram = () => {
    console.log('Stopping program');
    // Logic to stop the program goes here
  };

  // Example function to update motor positions (can be used with setInterval or WebSocket data)
  const updateMotorPositions = () => {
    setMotorPositions((prevPositions) =>
      prevPositions.map((motor, index) => ({
        current: motor.current + Math.floor(Math.random() * 10), // Simulate position change
        target: motor.target,
      }))
    );
    setCurrentWaypoint((prev) => prev + 1); // Simulate waypoint increment
  };

  return (
    <Box sx={{ width: '100%', maxWidth: '800px', padding: 3, marginX: 'auto' }}>
      {/* Program Selection Dropdown */}
      <FormControl fullWidth margin="normal">
        <InputLabel>Program</InputLabel>
        <Select
          value={selectedProgram}
          onChange={handleProgramChange}
          label="Program"
        >
          {availablePrograms.map((program, index) => (
            <MenuItem key={index} value={program}>
              {program}
            </MenuItem>
          ))}
        </Select>
      </FormControl>

      {/* Start/Stop Button */}
      <Button
        variant="contained"
        color={isRunning ? 'secondary' : 'primary'}
        onClick={handleToggleRun}
        sx={{ marginBottom: 3 }}
      >
        {isRunning ? 'Stop' : 'Start'}
      </Button>

      {/* Current Waypoint Display */}
      <Typography variant="h6" gutterBottom>
        Current Waypoint: {currentWaypoint}
      </Typography>

      {/* Motor Positions Table */}
      <TableContainer component={Paper}>
        <Table>
          <TableHead>
            <TableRow>
              <TableCell>Motor</TableCell>
              <TableCell align="center">Current Position</TableCell>
              <TableCell align="center">Target Position</TableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            {motorPositions.map((motor, index) => (
              <TableRow key={index}>
                <TableCell component="th" scope="row">
                  {motorNames[index]}
                </TableCell>
                <TableCell align="center" color="primary">
                  {motor.current}
                </TableCell>
                <TableCell align="center" color="secondary">
                  {motor.target}
                </TableCell>
              </TableRow>
            ))}
          </TableBody>
        </Table>
      </TableContainer>
    </Box>
  );
};

export default RunMode;