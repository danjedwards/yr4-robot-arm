import React, { useState } from 'react';
import MotorSlider from './MotorSlider';
import motorNames from '../globals';
import { Box, Button, Dialog, DialogTitle, DialogContent, DialogActions, TextField, Select, MenuItem, FormControl, InputLabel } from '@mui/material';
import SpinBox from './SpinBox';

const ProgrammingView = ({ motorPositions, handleSliderChange, sendSignal}) => {
  const [waypointStep, setWaypointStep] = useState(1);
  const [isDialogOpen, setDialogOpen] = useState(false);
  const [programIndex, setProgramIndex] = useState(0);
  const [programName, setProgramName] = useState('');

  // Function to open the dialog
  const handleSaveWaypoint = () => {
    setDialogOpen(true);
  };

  // Function to handle dialog close
  const handleDialogClose = () => {
    setDialogOpen(false);
  };

  // Function to handle program save
  const handleProgramSave = () => {
    console.log(`Saved Program - Index: ${programIndex}, Name: ${programName}`);
    // Additional save logic goes here
    setDialogOpen(false);
  };

  return (
    <Box sx={{ width: '100%', maxWidth: '800px', padding: 3, marginX: 'auto' }}>
      <Box display="flex" justifyContent="center" marginBottom={3} sx={{ width: '100%' }}>
        <SpinBox 
          label="Waypoint Step" 
          min={0} 
          max={100} 
          step={1} 
          value={waypointStep} 
          onChange={setWaypointStep} 
        />
      </Box>

      <Box display="flex" flexDirection="column" alignItems="center" gap={3} marginBottom={4} sx={{ width: '100%' }}>
        {motorPositions.map((position, index) => (
          <MotorSlider
            key={index}
            value={position}
            onChange={(e, newValue) => handleSliderChange(index, newValue)}
            label={motorNames[index]}
          />
        ))}
      </Box>

      <Box>
        <Button variant="contained" color="primary" onClick={sendSignal} margin={20}>
          Set Waypoint
        </Button>

        <Button variant="contained" color="primary" onClick={handleSaveWaypoint}>
          Save Program
        </Button>
      </Box>

      <Dialog open={isDialogOpen} onClose={handleDialogClose}>
        <DialogTitle>Save Program</DialogTitle>
        <DialogContent>
          <FormControl fullWidth margin="normal">
            <InputLabel>Program Index</InputLabel>
            <Select
              value={programIndex}
              onChange={(e) => setProgramIndex(e.target.value)}
              label="Program Index"
            >
              {[0, 1, 2, 3, 4].map((index) => (
                <MenuItem key={index} value={index}>
                  {index}
                </MenuItem>
              ))}
            </Select>
          </FormControl>
          <TextField
            fullWidth
            margin="normal"
            label="Program Name"
            value={programName}
            onChange={(e) => setProgramName(e.target.value)}
          />
        </DialogContent>
        <DialogActions>
          <Button onClick={handleDialogClose} color="secondary">
            Cancel
          </Button>
          <Button onClick={handleProgramSave} color="primary" variant="contained">
            Save
          </Button>
        </DialogActions>
      </Dialog>
    </Box>
  );
};

export default ProgrammingView;
