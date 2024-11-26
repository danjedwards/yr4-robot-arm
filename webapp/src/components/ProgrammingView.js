import React, { useState } from 'react';
import MotorSlider from './MotorSlider';
import motorNames from '../globals';
import { Box, Button, Dialog, DialogTitle, DialogContent, DialogActions, TextField, Select, MenuItem, FormControl, InputLabel } from '@mui/material';
import SpinBox from './SpinBox';

const ProgrammingView = ({ waypoint, handleSliderChange, sendWaypoint, saveProgram}) => {
  const [waypointStep, setWaypointStep] = useState(1);
  const [isDialogOpen, setDialogOpen] = useState(false);
  const [programIndex, setProgramIndex] = useState(0);
  const [programName, setProgramName] = useState('');

  const handleSaveWaypoint = () => {
    setDialogOpen(true);
  };

  const handleDialogClose = () => {
    setDialogOpen(false);
  };

  const handleProgramSave = () => {
    console.log(`Saved Program - Index: ${programIndex}, Name: ${programName}`);
    setDialogOpen(false);
    // saveProgram(programIndex, programName);
    saveProgram(programIndex, programName);
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
        {waypoint.map((position, index) => (
          <MotorSlider
            key={index}
            value={position}
            onChange={(e, newValue) => handleSliderChange(index, newValue)}
            label={motorNames[index]}
          />
        ))}
      </Box>

      <Box display="flex" justifyContent="center" alignItems="center" sx={{ marginY: 3, gap: 2 }}>
        <Button
          variant="contained"
          color="primary"
          onClick={sendWaypoint}
          sx={{
            paddingX: 3, 
            paddingY: 2, 
            fontSize: '1.2rem',
          }}
        >
          Set Waypoint
        </Button>

        <Button
          variant="contained"
          color="primary"
          onClick={handleSaveWaypoint}
          sx={{
            paddingX: 3,
            paddingY: 2,
            fontSize: '1.2rem',
          }}
        >
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
