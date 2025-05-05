import { Flight, Booking } from '../types';

// Sample flights data
const sampleFlights: Flight[] = [
  {
    id: '1',
    airline: 'Skyward Airlines',
    flightNumber: 'SA101',
    departureCity: 'New York',
    destinationCity: 'London',
    departureTime: '2025-06-10T08:00:00',
    arrivalTime: '2025-06-10T20:00:00',
    price: 450,
    seats: 120,
  },
  {
    id: '2',
    airline: 'Skyward Airlines',
    flightNumber: 'SA205',
    departureCity: 'Los Angeles',
    destinationCity: 'Tokyo',
    departureTime: '2025-06-12T10:30:00',
    arrivalTime: '2025-06-13T14:30:00',
    price: 850,
    seats: 200,
  },
  {
    id: '3',
    airline: 'Skyward Express',
    flightNumber: 'SE311',
    departureCity: 'Chicago',
    destinationCity: 'Paris',
    departureTime: '2025-06-15T14:15:00',
    arrivalTime: '2025-06-16T07:45:00',
    price: 620,
    seats: 150,
  },
  {
    id: '4',
    airline: 'Skyward Express',
    flightNumber: 'SE422',
    departureCity: 'Miami',
    destinationCity: 'Barcelona',
    departureTime: '2025-06-18T09:20:00',
    arrivalTime: '2025-06-18T23:50:00',
    price: 580,
    seats: 180,
  },
  {
    id: '5',
    airline: 'Skyward Connect',
    flightNumber: 'SC505',
    departureCity: 'Boston',
    destinationCity: 'Rome',
    departureTime: '2025-06-20T11:45:00',
    arrivalTime: '2025-06-21T03:30:00',
    price: 670,
    seats: 140,
  },
  {
    id: '6',
    airline: 'Skyward Connect',
    flightNumber: 'SC610',
    departureCity: 'Seattle',
    destinationCity: 'Sydney',
    departureTime: '2025-06-22T16:10:00',
    arrivalTime: '2025-06-24T06:45:00',
    price: 920,
    seats: 220,
  }
];

// Get all flights
export const getFlights = (): Flight[] => {
  return sampleFlights;
};

// Search flights
export const searchFlights = (from: string, to: string, date: string): Flight[] => {
  return sampleFlights.filter(flight => {
    const matchFrom = from ? flight.departureCity.toLowerCase().includes(from.toLowerCase()) : true;
    const matchTo = to ? flight.destinationCity.toLowerCase().includes(to.toLowerCase()) : true;
    const matchDate = date ? new Date(flight.departureTime).toDateString() === new Date(date).toDateString() : true;
    return matchFrom && matchTo && matchDate;
  });
};

// Get flight by ID
export const getFlightById = (id: string): Flight | undefined => {
  return sampleFlights.find(flight => flight.id === id);
};

// Store bookings
const BOOKINGS_KEY = 'skyward_bookings';

// Get bookings
export const getBookings = (): Booking[] => {
  const bookingsJson = localStorage.getItem(BOOKINGS_KEY);
  return bookingsJson ? JSON.parse(bookingsJson) : [];
};

// Save booking
export const saveBooking = (booking: Booking): void => {
  const bookings = getBookings();
  localStorage.setItem(BOOKINGS_KEY, JSON.stringify([...bookings, booking]));
};

// Generate confirmation code
export const generateConfirmationCode = (): string => {
  return 'SKY' + Math.random().toString(36).substring(2, 8).toUpperCase();
};

// Simulate sending email
export const sendConfirmationEmail = (booking: Booking): void => {
  console.log(`Email sent to ${booking.email} with confirmation code ${booking.confirmationCode}`);
};