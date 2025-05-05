import React, { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { Plane, MapPin, Calendar, Search } from 'lucide-react';
import Button from '../components/Button';

const HomePage: React.FC = () => {
  const navigate = useNavigate();
  const [from, setFrom] = useState('');
  const [to, setTo] = useState('');
  const [date, setDate] = useState('');

  const handleSearch = (e: React.FormEvent) => {
    e.preventDefault();
    navigate(`/flights?from=${from}&to=${to}&date=${date}`);
  };

  return (
    <div className="flex flex-col min-h-screen">
      {/* Hero Section */}
      <div 
        className="min-h-screen relative flex items-center justify-center" 
        style={{
          backgroundImage: 'url(https://images.pexels.com/photos/912050/pexels-photo-912050.jpeg?auto=compress&cs=tinysrgb&w=1260&h=750&dpr=2)',
          backgroundSize: 'cover',
          backgroundPosition: 'center',
        }}
      >
        <div className="absolute inset-0 bg-black bg-opacity-50"></div>
        
        <div className="container mx-auto px-4 z-10 text-center">
          <div className="max-w-3xl mx-auto">
            <div className="flex items-center justify-center mb-4">
              <Plane className="h-10 w-10 text-[#d0e7d2] mr-2" />
              <h1 className="text-4xl md:text-5xl font-bold text-white">Skyward</h1>
            </div>
            <p className="text-xl md:text-2xl text-white mb-8">Explore the world with ease and comfort</p>
            
            <div className="bg-white p-6 rounded-lg shadow-lg">
              <form onSubmit={handleSearch}>
                <div className="grid grid-cols-1 md:grid-cols-3 gap-4 mb-4">
                  <div className="relative">
                    <MapPin className="absolute left-3 top-1/2 transform -translate-y-1/2 text-gray-400" />
                    <input
                      type="text"
                      placeholder="From"
                      className="w-full pl-10 pr-4 py-3 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-[#d0e7d2]"
                      value={from}
                      onChange={(e) => setFrom(e.target.value)}
                    />
                  </div>
                  
                  <div className="relative">
                    <MapPin className="absolute left-3 top-1/2 transform -translate-y-1/2 text-gray-400" />
                    <input
                      type="text"
                      placeholder="To"
                      className="w-full pl-10 pr-4 py-3 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-[#d0e7d2]"
                      value={to}
                      onChange={(e) => setTo(e.target.value)}
                    />
                  </div>
                  
                  <div className="relative">
                    <Calendar className="absolute left-3 top-1/2 transform -translate-y-1/2 text-gray-400" />
                    <input
                      type="date"
                      className="w-full pl-10 pr-4 py-3 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-[#d0e7d2]"
                      value={date}
                      onChange={(e) => setDate(e.target.value)}
                    />
                  </div>
                </div>
                
                <Button type="submit" fullWidth>
                  <div className="flex items-center justify-center">
                    <Search className="mr-2 h-5 w-5" />
                    Search Flights
                  </div>
                </Button>
              </form>
            </div>
          </div>
        </div>
      </div>
      
      {/* Features Section */}
      <section className="py-16 bg-gray-50">
        <div className="container mx-auto px-4">
          <h2 className="text-3xl font-bold text-center mb-12 text-[#043927]">Why Choose Skyward</h2>
          
          <div className="grid grid-cols-1 md:grid-cols-3 gap-8">
            <div className="bg-white p-8 rounded-lg shadow-md text-center hover:shadow-lg transition-shadow duration-300">
              <div className="bg-[#d0e7d2] p-4 rounded-full inline-flex justify-center items-center mb-4">
                <Plane className="h-6 w-6 text-[#043927]" />
              </div>
              <h3 className="text-xl font-semibold mb-2 text-[#043927]">Best Prices</h3>
              <p className="text-gray-600">Competitive prices for all destinations, with special discounts for early bookings.</p>
            </div>
            
            <div className="bg-white p-8 rounded-lg shadow-md text-center hover:shadow-lg transition-shadow duration-300">
              <div className="bg-[#d0e7d2] p-4 rounded-full inline-flex justify-center items-center mb-4">
                <Search className="h-6 w-6 text-[#043927]" />
              </div>
              <h3 className="text-xl font-semibold mb-2 text-[#043927]">Easy Booking</h3>
              <p className="text-gray-600">Simple, fast, and secure booking process that saves you time and hassle.</p>
            </div>
            
            <div className="bg-white p-8 rounded-lg shadow-md text-center hover:shadow-lg transition-shadow duration-300">
              <div className="bg-[#d0e7d2] p-4 rounded-full inline-flex justify-center items-center mb-4">
                <Calendar className="h-6 w-6 text-[#043927]" />
              </div>
              <h3 className="text-xl font-semibold mb-2 text-[#043927]">Flexible Dates</h3>
              <p className="text-gray-600">Change your travel dates with minimal fees, giving you peace of mind.</p>
            </div>
          </div>
        </div>
      </section>
      
      {/* Destinations Section */}
      <section className="py-16">
        <div className="container mx-auto px-4">
          <h2 className="text-3xl font-bold text-center mb-12 text-[#043927]">Popular Destinations</h2>
          
          <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
            <div className="rounded-lg overflow-hidden shadow-md hover:shadow-xl transition-shadow duration-300 group">
              <div className="relative">
                <img
                  src="https://images.pexels.com/photos/1525612/pexels-photo-1525612.jpeg?auto=compress&cs=tinysrgb&w=1260&h=750&dpr=2"
                  alt="New York"
                  className="w-full h-64 object-cover transition-transform duration-500 group-hover:scale-110"
                />
                <div className="absolute inset-0 bg-gradient-to-t from-black to-transparent opacity-70"></div>
                <div className="absolute bottom-0 left-0 p-6">
                  <h3 className="text-2xl font-bold text-white">New York</h3>
                  <p className="text-white opacity-90">From $299</p>
                </div>
              </div>
            </div>
            
            <div className="rounded-lg overflow-hidden shadow-md hover:shadow-xl transition-shadow duration-300 group">
              <div className="relative">
                <img
                  src="https://images.pexels.com/photos/699466/pexels-photo-699466.jpeg?auto=compress&cs=tinysrgb&w=1260&h=750&dpr=2"
                  alt="London"
                  className="w-full h-64 object-cover transition-transform duration-500 group-hover:scale-110"
                />
                <div className="absolute inset-0 bg-gradient-to-t from-black to-transparent opacity-70"></div>
                <div className="absolute bottom-0 left-0 p-6">
                  <h3 className="text-2xl font-bold text-white">London</h3>
                  <p className="text-white opacity-90">From $450</p>
                </div>
              </div>
            </div>
            
            <div className="rounded-lg overflow-hidden shadow-md hover:shadow-xl transition-shadow duration-300 group">
              <div className="relative">
                <img
                  src="https://images.pexels.com/photos/1510595/pexels-photo-1510595.jpeg?auto=compress&cs=tinysrgb&w=1260&h=750&dpr=2"
                  alt="Tokyo"
                  className="w-full h-64 object-cover transition-transform duration-500 group-hover:scale-110"
                />
                <div className="absolute inset-0 bg-gradient-to-t from-black to-transparent opacity-70"></div>
                <div className="absolute bottom-0 left-0 p-6">
                  <h3 className="text-2xl font-bold text-white">Tokyo</h3>
                  <p className="text-white opacity-90">From $850</p>
                </div>
              </div>
            </div>
          </div>
        </div>
      </section>
      
      {/* CTA Section */}
      <section 
        className="py-20 relative" 
        style={{
          backgroundImage: 'url(https://images.pexels.com/photos/358319/pexels-photo-358319.jpeg?auto=compress&cs=tinysrgb&w=1260&h=750&dpr=2)',
          backgroundSize: 'cover',
          backgroundPosition: 'center',
          backgroundAttachment: 'fixed',
        }}
      >
        <div className="absolute inset-0 bg-[#043927] bg-opacity-80"></div>
        
        <div className="container mx-auto px-4 relative z-10 text-center">
          <h2 className="text-3xl md:text-4xl font-bold text-white mb-4">Ready to explore the world?</h2>
          <p className="text-xl text-white mb-8 max-w-2xl mx-auto">Book your flight today and experience the comfort and reliability of Skyward Airlines.</p>
          <Button onClick={() => navigate('/flights')} variant="secondary" className="inline-flex">
            Book Now
          </Button>
        </div>
      </section>
    </div>
  );
};

export default HomePage;