export interface User {
  id: string;
  email: string;
  name?: string;
}

export interface Flight {
  id: string;
  airline: string;
  flightNumber: string;
  departureCity: string;
  destinationCity: string;
  departureTime: string;
  arrivalTime: string;
  price: number;
  seats: number;
}

export interface Booking {
  id: string;
  userId: string;
  flightId: string;
  fullName: string;
  email: string;
  phone: string;
  departureCity: string;
  destinationCity: string;
  travelDate: string;
  passengers: number;
  paymentMethod: 'online' | 'offline';
  paymentStatus: 'pending' | 'completed';
  confirmationCode: string;
  createdAt: string;
}

export interface PaymentDetails {
  cardNumber: string;
  expiryDate: string;
  cvv: string;
  nameOnCard: string;
}