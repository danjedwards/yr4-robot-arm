import React, { useState } from 'react';
import { Box, TextField, IconButton } from '@mui/material';
import AddIcon from '@mui/icons-material/Add';
import RemoveIcon from '@mui/icons-material/Remove';

const SpinBox = ({ label, min = 0, max = 99, step = 1, value, onChange }) => {
  const handleIncrement = () => {
    onChange(Math.min(value + step, max));
  };

  const handleDecrement = () => {
    onChange(Math.max(value - step, min));
  };

  const handleChange = (event) => {
    const newValue = parseInt(event.target.value, 10);
    if (!isNaN(newValue) && newValue >= min && newValue <= max) {
      onChange(newValue);
    } else if (event.target.value === '') {
      onChange(''); // Allow empty for clearing, but revert if out of bounds
    }
  };

  const handleBlur = () => {
    // Revert to min or max if out of bounds after input
    if (value === '' || value < min) {
      onChange(min);
    } else if (value > max) {
      onChange(max);
    }
  };

  return (
    <Box display="flex" alignItems="center" gap={1} sx={{ width: '80%' }}>
      <IconButton onClick={handleDecrement} disabled={value <= min}>
        <RemoveIcon />
      </IconButton>

      <TextField
        label={label}
        type="text" // Use text to hide arrows
        inputMode="numeric" // Suggest numeric keypad on mobile
        pattern="[0-9]*" // Allow only numbers (no arrows)
        value={value}
        onChange={handleChange}
        onBlur={handleBlur}
        slotProps={{ input: { min, max } }}
        sx={{
          flexGrow: 1, // Make the TextField fill the remaining space
          textAlign: 'center',
          '& .MuiInputBase-input': {
            fontSize: '1.5rem', // Increase font size
            textAlign: 'center',
          },
        }}
      />

      <IconButton onClick={handleIncrement} disabled={value >= max}>
        <AddIcon />
      </IconButton>
    </Box>
  );
};

export default SpinBox;
