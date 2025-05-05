import React, { useState, useEffect } from 'react';
import { useNavigate } from 'react-router-dom';
import { getBookings } from '../utils/flightUtils';
import { getFlightById } from '../utils/flightUtils';
import { isAuthenticated, getCurrentUser } from '../utils/authUtils';
import { Booking, Flight } from '../types';
import { Plane, Calendar, MapPin, Clock } from 'lucide-react';
import Button from '../components/Button';

const DashboardPage: React.FC = () => {
  const navigate = useNavigate();
  const [bookings, setBookings] = useState<(Booking & { flight?: Flight })[]>([]);
  const [isLoading, setIsLoading] = useState(true);
  
  useEffect(() => {
    // Check if user is authenticated
    if (!isAuthenticated()) {
      navigate('/login', { state: { redirectTo: '/dashboard' } });
      return;
    }
    
    // Load user bookings
    loadBookings();
  }, [navigate]);
  
  const loadBookings = () => {
    setIsLoading(true);
    
    const user = getCurrentUser();
    if (!user) {
      navigate('/login');
      return;
    }
    
    // Get all bookings for the current user
    const allBookings = getBookings();
    const userBookings = allBookings.filter(booking => booking.userId === user.id);
    
    // Add flight details to each booking
    const bookingsWithFlights = userBookings.map(booking => {
      const flight = getFlightById(booking.flightId);
      return {
        ...booking,
        flight,
      };
    });
    
    // Sort by creation date (newest first)
    bookingsWithFlights.sort((a, b) => {
      return new Date(b.createdAt).getTime() - new Date(a.createdAt).getTime();
    });
    
    setBookings(bookingsWithFlights);
    setIsLoading(false);
  };
  
  const formatDate = (dateString: string) => {
    const date = new Date(dateString);
    return date.toLocaleDateString('en-US', {
      year: 'numeric',
      month: 'short',
      day: 'numeric',
    });
  };
  
  return (
    <div className="min-h-screen bg-gray-50 pt-24 pb-16">
      <div className="container mx-auto px-4">
        <div className="max-w-4xl mx-auto">
          <h1 className="text-3xl font-bold text-[#043927] mb-8">My Bookings</h1>
          
          {isLoading ? (
            <div className="text-center py-12">
              <p className="text-gray-600">Loading your bookings...</p>
            </div>
          ) : bookings.length === 0 ? (
            <div className="bg-white rounded-lg shadow-md p-8 text-center">
              <Plane className="h-12 w-12 text-gray-400 mx-auto mb-4" />
              <h2 className="text-xl font-semibold mb-2">No Bookings Found</h2>
              <p className="text-gray-600 mb-6">You haven't made any bookings yet.</p>
              <Button onClick={() => navigate('/flights')} variant="primary">
                Search Flights
              </Button>
            </div>
          ) : (
            <div className="space-y-6">
              {bookings.map((booking) => (
                <div key={booking.id} className="bg-white rounded-lg shadow-md overflow-hidden">
                  <div className="bg-[#043927] text-white p-4">
                    <div className="flex justify-between items-center">
                      <div className="flex items-center">
                        <Plane className="h-5 w-5 mr-2" />
                        <span className="font-semibold">{booking.flight?.airline} {booking.flight?.flightNumber}</span>
                      </div>
                      <div className="text-sm bg-[#d0e7d2] text-[#043927] px-3 py-1 rounded-full font-medium">
                        {booking.paymentStatus === 'completed' ? 'Paid' : 'Payment Pending'}
                      </div>
                    </div>
                  </div>
                  
                  <div className="p-6">
                    <div className="flex justify-between items-start mb-4">
                      <div>
                        <h3 className="text-lg font-semibold mb-1">Confirmation # {booking.confirmationCode}</h3>
                        <p className="text-gray-500 text-sm">Booked on {formatDate(booking.createdAt)}</p>
                      </div>
                      <div className="text-lg font-bold text-[#043927]">
                        ${booking.flight ? booking.flight.price * booking.passengers : 'N/A'}
                      </div>
                    </div>
                    
                    <div className="border-t border-gray-100 pt-4">
                      <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
                        <div className="flex items-start">
                          <MapPin className="h-5 w-5 text-gray-400 mr-2 mt-0.5" />
                          <div>
                            <p className="text-sm text-gray-500">From - To</p>
                            <p className="font-medium">{booking.departureCity} - {booking.destinationCity}</p>
                          </div>
                        </div>
                        
                        <div className="flex items-start">
                          <Calendar className="h-5 w-5 text-gray-400 mr-2 mt-0.5" />
                          <div>
                            <p className="text-sm text-gray-500">Travel Date</p>
                            <p className="font-medium">{formatDate(booking.travelDate)}</p>
                          </div>
                        </div>
                        
                        <div className="flex items-start">
                          <Clock className="h-5 w-5 text-gray-400 mr-2 mt-0.5" />
                          <div>
                            <p className="text-sm text-gray-500">Departure - Arrival</p>
                            <p className="font-medium">
                              {booking.flight ? (
                                <>
                                  {new Date(booking.flight.departureTime).toLocaleTimeString([], {hour: '2-digit', minute:'2-digit'})}
                                  {' - '}
                                  {new Date(booking.flight.arrivalTime).toLocaleTimeString([], {hour: '2-digit', minute:'2-digit'})}
                                </>
                              ) : (
                                'N/A'
                              )}
                            </p>
                          </div>
                        </div>
                        
                        <div className="flex items-start">
                          <Plane className="h-5 w-5 text-gray-400 mr-2 mt-0.5" />
                          <div>
                            <p className="text-sm text-gray-500">Passengers</p>
                            <p className="font-medium">{booking.passengers} {booking.passengers === 1 ? 'passenger' : 'passengers'}</p>
                          </div>
                        </div>
                      </div>
                    </div>
                    
                    {booking.paymentMethod === 'offline' && booking.paymentStatus === 'pending' && (
                      <div className="mt-4 p-4 bg-yellow-50 rounded-md text-sm border border-yellow-200">
                        <p className="font-medium text-yellow-800 mb-1">Payment Pending</p>
                        <p className="text-yellow-700">
                          Please visit the nearest airport ticket counter to complete your payment.
                          Present your confirmation code: <span className="font-bold">{booking.confirmationCode}</span>
                        </p>
                      </div>
                    )}
                  </div>
                </div>
              ))}
            </div>
          )}
        </div>
      </div>
    </div>
  );
};

export default DashboardPage;