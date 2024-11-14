import React from 'react';
import ReactDOM from 'react-dom';
import App from './App';
import { ThemeProvider, CssBaseline } from '@mui/material';
import theme from './theme'; // Import your theme configuration

ReactDOM.render(
  <ThemeProvider theme={theme}>
    <CssBaseline /> {/* Ensures base styles are applied for MUI */}
    <App />
  </ThemeProvider>,
  document.getElementById('root')
);
