import React from 'react';
import { Box, Slider, Typography, Tooltip } from '@mui/material';

const MotorSlider = ({ value, onChange, label }) => {
  return (
    <Box sx={{ width: '100%', marginBottom: 4 }}>
      {/* Label above the slider */}
      <Typography variant="h6" color="secondary" align="center" sx={{ marginBottom: 1 }}>
        {label}
      </Typography>

      {/* Slider with Tooltip below */}
      <Tooltip title={value} placement="bottom" arrow>
        <Slider
          value={value}
          onChange={onChange}
          min={410}
          max={2048}
          step={1}
          sx={{
            width: '100%',
            color: 'secondary.main',
            '& .MuiSlider-thumb': {
              backgroundColor: 'secondary.main',
            },
            '& .MuiSlider-track': {
              backgroundColor: 'primary.main',
            },
          }}
        />
      </Tooltip>
    </Box>
  );
};

export default MotorSlider;
