// src/services/apiClient.ts
const API_BASE = 'http://127.0.0.1:5000';

interface FlightSearchPayload {
  source: string;
  destination: string;
  day: string;
  departure_time: string;
}

export async function searchFlights(payload: FlightSearchPayload) {
  const res = await fetch(`${API_BASE}/api/data`, {
    method: 'POST',
    mode: 'cors',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(payload),
  });
  if (!res.ok) throw new Error(`Search flights failed (${res.status})`);
  return res.json(); // expect Flight[]
}

export async function fetchBookings(userId: string) {
  const res = await fetch(`${API_BASE}/api/bookings?userId=${userId}`, {
    headers: { 'Content-Type': 'application/json' },
    credentials: 'include',
  });
  if (!res.ok) throw new Error(`Fetch bookings failed (${res.status})`);
  return res.json(); // expect Booking[]
}

export async function createBooking(data: any) {
  const res = await fetch(`${API_BASE}/api/bookings`, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(data),
  });
  if (!res.ok) throw new Error(`Create booking failed (${res.status})`);
  return res.json(); // expect Booking
}

// similarly loginUser, registerUser, getFlightById...
