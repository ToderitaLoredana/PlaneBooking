import { StrictMode } from 'react';
import { createRoot } from 'react-dom/client';
import App from './App.tsx';
import './index.css';

// Initialize some sample users
const initializeApp = () => {
  // Check if users have been initialized
  if (!localStorage.getItem('skyward_users')) {
    // Create sample users
    const sampleUsers = [
      {
        id: '1',
        email: 'demo@example.com',
        password: 'cGFzc3dvcmQxMjM=', // base64 for "password123"
      }
    ];
    localStorage.setItem('skyward_users', JSON.stringify(sampleUsers));
  }
};

// Initialize app
initializeApp();

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <App />
  </StrictMode>
);