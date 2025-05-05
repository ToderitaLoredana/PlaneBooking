import { User } from '../types';

// Simulate a database of users
const USERS_KEY = 'skyward_users';
const CURRENT_USER_KEY = 'skyward_current_user';

// Get users from local storage
export const getUsers = (): User[] => {
  const usersJson = localStorage.getItem(USERS_KEY);
  return usersJson ? JSON.parse(usersJson) : [];
};

// Save users to local storage
export const saveUsers = (users: User[]) => {
  localStorage.setItem(USERS_KEY, JSON.stringify(users));
};

// Register a new user
export const registerUser = (email: string, password: string): User | null => {
  // Check if user already exists
  const users = getUsers();
  if (users.find(user => user.email === email)) {
    return null;
  }

  // Hash password (in a real app, this would be done server-side)
  const hashedPassword = btoa(password); // Base64 encoding as a simple "hash" simulation

  // Create new user
  const newUser: User & { password: string } = {
    id: Date.now().toString(),
    email,
    password: hashedPassword,
  };

  // Save user
  saveUsers([...users, newUser]);

  // Return user without password
  const { password: _, ...userWithoutPassword } = newUser;
  return userWithoutPassword;
};

// Login
export const loginUser = (email: string, password: string): User | null => {
  const users = getUsers();
  const user = users.find(u => u.email === email) as (User & { password: string }) | undefined;
  
  if (!user) {
    return null;
  }

  // Check password
  const hashedPassword = btoa(password);
  if (user.password !== hashedPassword) {
    return null;
  }

  // Set current user
  const { password: _, ...userWithoutPassword } = user;
  localStorage.setItem(CURRENT_USER_KEY, JSON.stringify(userWithoutPassword));
  
  return userWithoutPassword;
};

// Get current user
export const getCurrentUser = (): User | null => {
  const userJson = localStorage.getItem(CURRENT_USER_KEY);
  return userJson ? JSON.parse(userJson) : null;
};

// Logout
export const logoutUser = () => {
  localStorage.removeItem(CURRENT_USER_KEY);
};

// Check if user is authenticated
export const isAuthenticated = (): boolean => {
  return !!getCurrentUser();
};