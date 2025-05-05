import React, { useState, useEffect } from 'react';
import { useParams, useNavigate } from 'react-router-dom';
import { CreditCard, MapPin, Calendar, Users, Phone, Mail, CheckCircle } from 'lucide-react';
import { getFlightById, generateConfirmationCode, saveBooking, sendConfirmationEmail } from '../utils/flightUtils';
import { isAuthenticated, getCurrentUser } from '../utils/authUtils';
import { Flight, Booking, PaymentDetails } from '../types';
import Button from '../components/Button';
import Input from '../components/Input';

const BookingPage: React.FC = () => {
  const { flightId } = useParams<{ flightId: string }>();
  const navigate = useNavigate();
  const [flight, setFlight] = useState<Flight | null>(null);
  const [step, setStep] = useState<'details' | 'payment' | 'confirmation'>('details');
  
  // Form states
  const [fullName, setFullName] = useState('');
  const [email, setEmail] = useState('');
  const [phone, setPhone] = useState('');
  const [passengers, setPassengers] = useState(1);
  const [travelDate, setTravelDate] = useState('');
  const [paymentMethod, setPaymentMethod] = useState<'online' | 'offline'>('online');
  
  // Payment details
  const [cardNumber, setCardNumber] = useState('');
  const [expiryDate, setExpiryDate] = useState('');
  const [cvv, setCvv] = useState('');
  const [nameOnCard, setNameOnCard] = useState('');
  
  // Booking confirmation
  const [booking, setBooking] = useState<Booking | null>(null);
  const [isLoading, setIsLoading] = useState(false);
  
  useEffect(() => {
    // Check if user is authenticated
    if (!isAuthenticated()) {
      navigate('/login', { state: { redirectTo: `/booking/${flightId}` } });
      return;
    }
    
    // Get flight details
    if (flightId) {
      const flightDetails = getFlightById(flightId);
      if (flightDetails) {
        setFlight(flightDetails);
        // Set default travel date to the flight departure date
        const departureDate = new Date(flightDetails.departureTime);
        setTravelDate(departureDate.toISOString().split('T')[0]);
        
        // Pre-fill user info if available
        const user = getCurrentUser();
        if (user) {
          setEmail(user.email || '');
          setFullName(user.name || '');
        }
      } else {
        navigate('/flights');
      }
    }
  }, [flightId, navigate]);
  
  const formatDate = (dateString: string) => {
    const date = new Date(dateString);
    return date.toLocaleDateString('en-US', {
      weekday: 'short',
      year: 'numeric',
      month: 'short',
      day: 'numeric',
    });
  };
  
  const formatTime = (dateString: string) => {
    const date = new Date(dateString);
    return date.toLocaleTimeString('en-US', {
      hour: '2-digit',
      minute: '2-digit',
    });
  };
  
  const handleDetailsSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    if (paymentMethod === 'online') {
      setStep('payment');
    } else {
      processBooking();
    }
  };
  
  const handlePaymentSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    processBooking();
  };
  
  const processBooking = () => {
    if (!flight || !flightId) return;
    
    setIsLoading(true);
    
    // Simulate API call with timeout
    setTimeout(() => {
      const user = getCurrentUser();
      if (!user) {
        navigate('/login');
        return;
      }
      
      const confirmationCode = generateConfirmationCode();
      
      const newBooking: Booking = {
        id: Date.now().toString(),
        userId: user.id,
        flightId: flightId,
        fullName,
        email,
        phone,
        departureCity: flight.departureCity,
        destinationCity: flight.destinationCity,
        travelDate,
        passengers,
        paymentMethod,
        paymentStatus: paymentMethod === 'online' ? 'completed' : 'pending',
        confirmationCode,
        createdAt: new Date().toISOString(),
      };
      
      // Save booking
      saveBooking(newBooking);
      
      // Send confirmation email
      sendConfirmationEmail(newBooking);
      
      // Update state
      setBooking(newBooking);
      setStep('confirmation');
      setIsLoading(false);
    }, 1500);
  };
  
  const formatCardNumber = (value: string) => {
    // Remove all non-digit characters
    const cleaned = value.replace(/\D/g, '');
    // Add space after every 4 digits
    const formatted = cleaned.replace(/(\d{4})(?=\d)/g, '$1 ');
    // Limit to 19 characters (16 digits + 3 spaces)
    return formatted.slice(0, 19);
  };
  
  const formatExpiryDate = (value: string) => {
    // Remove all non-digit characters
    const cleaned = value.replace(/\D/g, '');
    // Format as MM/YY
    if (cleaned.length > 2) {
      return `${cleaned.slice(0, 2)}/${cleaned.slice(2, 4)}`;
    }
    return cleaned;
  };
  
  if (!flight) {
    return (
      <div className="min-h-screen pt-24 pb-16 flex items-center justify-center">
        <div className="text-center">
          <p className="text-xl text-gray-600">Loading flight details...</p>
        </div>
      </div>
    );
  }
  
  return (
    <div className="min-h-screen bg-gray-50 pt-24 pb-16">
      <div className="container mx-auto px-4">
        <div className="max-w-4xl mx-auto">
          {/* Progress Steps */}
          <div className="mb-8">
            <div className="flex items-center justify-center">
              <div className={`flex items-center ${step === 'details' ? 'text-[#043927] font-semibold' : 'text-gray-500'}`}>
                <div className={`w-8 h-8 rounded-full flex items-center justify-center ${step === 'details' ? 'bg-[#d0e7d2]' : 'bg-gray-200'}`}>
                  1
                </div>
                <span className="ml-2">Passenger Details</span>
              </div>
              
              <div className={`w-12 h-0.5 mx-2 ${step === 'details' ? 'bg-gray-300' : 'bg-[#d0e7d2]'}`}></div>
              
              <div className={`flex items-center ${step === 'payment' ? 'text-[#043927] font-semibold' : 'text-gray-500'}`}>
                <div className={`w-8 h-8 rounded-full flex items-center justify-center ${step === 'payment' ? 'bg-[#d0e7d2]' : step === 'confirmation' ? 'bg-[#d0e7d2]' : 'bg-gray-200'}`}>
                  2
                </div>
                <span className="ml-2">Payment</span>
              </div>
              
              <div className={`w-12 h-0.5 mx-2 ${step === 'confirmation' ? 'bg-[#d0e7d2]' : 'bg-gray-300'}`}></div>
              
              <div className={`flex items-center ${step === 'confirmation' ? 'text-[#043927] font-semibold' : 'text-gray-500'}`}>
                <div className={`w-8 h-8 rounded-full flex items-center justify-center ${step === 'confirmation' ? 'bg-[#d0e7d2]' : 'bg-gray-200'}`}>
                  3
                </div>
                <span className="ml-2">Confirmation</span>
              </div>
            </div>
          </div>
          
          {/* Flight Summary */}
          <div className="bg-white rounded-lg shadow-md p-6 mb-6">
            <h2 className="text-xl font-bold text-[#043927] mb-4">Flight Details</h2>
            
            <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
              <div>
                <p className="text-gray-600">Airline</p>
                <p className="font-semibold">{flight.airline}</p>
              </div>
              
              <div>
                <p className="text-gray-600">Flight Number</p>
                <p className="font-semibold">{flight.flightNumber}</p>
              </div>
              
              <div>
                <p className="text-gray-600">Departure</p>
                <p className="font-semibold">{flight.departureCity} ({formatTime(flight.departureTime)})</p>
                <p className="text-sm text-gray-500">{formatDate(flight.departureTime)}</p>
              </div>
              
              <div>
                <p className="text-gray-600">Arrival</p>
                <p className="font-semibold">{flight.destinationCity} ({formatTime(flight.arrivalTime)})</p>
                <p className="text-sm text-gray-500">{formatDate(flight.arrivalTime)}</p>
              </div>
              
              <div>
                <p className="text-gray-600">Price</p>
                <p className="font-semibold text-[#043927]">${flight.price} per person</p>
              </div>
              
              {passengers > 0 && (
                <div>
                  <p className="text-gray-600">Total Price</p>
                  <p className="font-bold text-[#043927]">${flight.price * passengers}</p>
                </div>
              )}
            </div>
          </div>
          
          {/* Passenger Details Form */}
          {step === 'details' && (
            <div className="bg-white rounded-lg shadow-md p-6">
              <h2 className="text-xl font-bold text-[#043927] mb-4">Passenger Details</h2>
              
              <form onSubmit={handleDetailsSubmit}>
                <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
                  <Input
                    id="fullName"
                    label="Full Name"
                    value={fullName}
                    onChange={(e) => setFullName(e.target.value)}
                    required
                  />
                  
                  <Input
                    id="email"
                    label="Email"
                    type="email"
                    value={email}
                    onChange={(e) => setEmail(e.target.value)}
                    required
                  />
                  
                  <Input
                    id="phone"
                    label="Phone Number"
                    type="tel"
                    value={phone}
                    onChange={(e) => setPhone(e.target.value)}
                    required
                  />
                  
                  <div className="mb-4">
                    <label htmlFor="passengers" className="block text-sm font-medium text-gray-700 mb-1">
                      Number of Passengers <span className="text-red-500">*</span>
                    </label>
                    <div className="relative">
                      <Users className="absolute left-3 top-1/2 transform -translate-y-1/2 text-gray-400" />
                      <input
                        id="passengers"
                        type="number"
                        min="1"
                        max="10"
                        value={passengers}
                        onChange={(e) => setPassengers(parseInt(e.target.value) || 1)}
                        className="w-full pl-10 pr-4 py-2 border border-gray-300 rounded-md shadow-sm focus:ring-2 focus:ring-[#d0e7d2] focus:border-[#d0e7d2] focus:outline-none transition duration-300"
                        required
                      />
                    </div>
                  </div>
                  
                  <div className="mb-4">
                    <label htmlFor="travelDate" className="block text-sm font-medium text-gray-700 mb-1">
                      Travel Date <span className="text-red-500">*</span>
                    </label>
                    <div className="relative">
                      <Calendar className="absolute left-3 top-1/2 transform -translate-y-1/2 text-gray-400" />
                      <input
                        id="travelDate"
                        type="date"
                        value={travelDate}
                        onChange={(e) => setTravelDate(e.target.value)}
                        className="w-full pl-10 pr-4 py-2 border border-gray-300 rounded-md shadow-sm focus:ring-2 focus:ring-[#d0e7d2] focus:border-[#d0e7d2] focus:outline-none transition duration-300"
                        required
                      />
                    </div>
                  </div>
                </div>
                
                <div className="mt-4 mb-6">
                  <p className="text-sm font-medium text-gray-700 mb-2">Payment Method <span className="text-red-500">*</span></p>
                  
                  <div className="flex flex-col sm:flex-row gap-4">
                    <label className={`flex items-center p-4 border rounded-md cursor-pointer transition-colors ${
                      paymentMethod === 'online' ? 'border-[#043927] bg-[#d0e7d2] bg-opacity-20' : 'border-gray-300'
                    }`}>
                      <input
                        type="radio"
                        name="paymentMethod"
                        value="online"
                        checked={paymentMethod === 'online'}
                        onChange={() => setPaymentMethod('online')}
                        className="hidden"
                      />
                      <CreditCard className={`mr-2 ${paymentMethod === 'online' ? 'text-[#043927]' : 'text-gray-400'}`} />
                      <div>
                        <span className={`font-medium ${paymentMethod === 'online' ? 'text-[#043927]' : 'text-gray-700'}`}>
                          Pay Online
                        </span>
                        <p className="text-sm text-gray-500">Credit/Debit Card</p>
                      </div>
                    </label>
                    
                    <label className={`flex items-center p-4 border rounded-md cursor-pointer transition-colors ${
                      paymentMethod === 'offline' ? 'border-[#043927] bg-[#d0e7d2] bg-opacity-20' : 'border-gray-300'
                    }`}>
                      <input
                        type="radio"
                        name="paymentMethod"
                        value="offline"
                        checked={paymentMethod === 'offline'}
                        onChange={() => setPaymentMethod('offline')}
                        className="hidden"
                      />
                      <MapPin className={`mr-2 ${paymentMethod === 'offline' ? 'text-[#043927]' : 'text-gray-400'}`} />
                      <div>
                        <span className={`font-medium ${paymentMethod === 'offline' ? 'text-[#043927]' : 'text-gray-700'}`}>
                          Pay at Airport
                        </span>
                        <p className="text-sm text-gray-500">Cash or Card</p>
                      </div>
                    </label>
                  </div>
                </div>
                
                <div className="flex justify-end">
                  <Button type="submit">
                    {paymentMethod === 'online' ? 'Proceed to Payment' : 'Complete Booking'}
                  </Button>
                </div>
              </form>
            </div>
          )}
          
          {/* Payment Form */}
          {step === 'payment' && (
            <div className="bg-white rounded-lg shadow-md p-6">
              <h2 className="text-xl font-bold text-[#043927] mb-4">Payment Details</h2>
              
              <form onSubmit={handlePaymentSubmit}>
                <div className="mb-6 p-4 border border-gray-200 rounded-md bg-gray-50">
                  <p className="font-medium">Amount to Pay: <span className="text-[#043927] font-bold">${flight.price * passengers}</span></p>
                </div>
                
                <Input
                  id="cardNumber"
                  label="Card Number"
                  value={cardNumber}
                  onChange={(e) => setCardNumber(formatCardNumber(e.target.value))}
                  placeholder="1234 5678 9012 3456"
                  required
                />
                
                <div className="grid grid-cols-2 gap-4">
                  <Input
                    id="expiryDate"
                    label="Expiry Date"
                    value={expiryDate}
                    onChange={(e) => setExpiryDate(formatExpiryDate(e.target.value))}
                    placeholder="MM/YY"
                    required
                  />
                  
                  <Input
                    id="cvv"
                    label="CVV"
                    value={cvv}
                    onChange={(e) => setCvv(e.target.value.replace(/\D/g, '').slice(0, 3))}
                    placeholder="123"
                    required
                  />
                </div>
                
                <Input
                  id="nameOnCard"
                  label="Name on Card"
                  value={nameOnCard}
                  onChange={(e) => setNameOnCard(e.target.value)}
                  required
                />
                
                <div className="mt-6 flex justify-between">
                  <Button 
                    type="button" 
                    variant="outline" 
                    onClick={() => setStep('details')}
                  >
                    Back
                  </Button>
                  
                  <Button type="submit" disabled={isLoading}>
                    {isLoading ? 'Processing...' : 'Complete Payment'}
                  </Button>
                </div>
              </form>
            </div>
          )}
          
          {/* Confirmation */}
          {step === 'confirmation' && booking && (
            <div className="bg-white rounded-lg shadow-md p-6 text-center">
              <div className="mb-6">
                <CheckCircle className="h-16 w-16 text-green-500 mx-auto mb-4" />
                <h2 className="text-2xl font-bold text-[#043927] mb-2">Booking Confirmed!</h2>
                <p className="text-gray-600">Your booking has been successfully processed.</p>
              </div>
              
              <div className="bg-gray-50 rounded-lg p-4 mb-6 text-left">
                <h3 className="font-semibold text-lg mb-2">Booking Details</h3>
                <p className="mb-1"><span className="text-gray-600">Confirmation Code:</span> <span className="font-bold">{booking.confirmationCode}</span></p>
                <p className="mb-1"><span className="text-gray-600">Flight:</span> {flight.airline} {flight.flightNumber}</p>
                <p className="mb-1"><span className="text-gray-600">From:</span> {booking.departureCity}</p>
                <p className="mb-1"><span className="text-gray-600">To:</span> {booking.destinationCity}</p>
                <p className="mb-1"><span className="text-gray-600">Travel Date:</span> {formatDate(booking.travelDate)}</p>
                <p className="mb-1"><span className="text-gray-600">Passengers:</span> {booking.passengers}</p>
                <p className="mb-1">
                  <span className="text-gray-600">Payment Status:</span>{' '}
                  <span className={`font-medium ${
                    booking.paymentStatus === 'completed' ? 'text-green-600' : 'text-orange-500'
                  }`}>
                    {booking.paymentStatus === 'completed' ? 'Paid' : 'Pending'}
                  </span>
                </p>
              </div>
              
              {booking.paymentMethod === 'offline' && (
                <div className="bg-yellow-50 border border-yellow-200 rounded-lg p-4 mb-6 text-left">
                  <h3 className="font-semibold text-lg mb-2">Payment Instructions</h3>
                  <p className="mb-2">Please visit the nearest airport ticket counter to complete your payment.</p>
                  <p className="mb-1"><span className="font-medium">Amount Due:</span> ${flight.price * booking.passengers}</p>
                  <p className="text-sm text-gray-600">Present your confirmation code when making the payment.</p>
                </div>
              )}
              
              <p className="mb-6 text-gray-600">
                A confirmation email has been sent to {booking.email} with all the details of your booking.
              </p>
              
              <div className="flex flex-col sm:flex-row justify-center gap-4">
                <Button onClick={() => navigate('/dashboard')} variant="primary">
                  View My Bookings
                </Button>
                <Button onClick={() => navigate('/')} variant="outline">
                  Return to Home
                </Button>
              </div>
            </div>
          )}
        </div>
      </div>
    </div>
  );
};

export default BookingPage;