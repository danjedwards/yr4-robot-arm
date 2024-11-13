// theme.js
import { createTheme } from '@mui/material/styles';

const theme = createTheme({
  palette: {
    mode: 'dark',
    primary: {
      main: '#ff7c00', // yellow color
    },
    secondary: {
      main: '#ffa500', // orange color
    },
    background: {
      default: '#1a1a1a', // dark background
      paper: '#222', // slightly lighter dark background for components
    },
    text: {
      primary: '#e0e0e0',
    },
  },
});

export default theme;
