import React, { useState, useEffect } from 'react';
import { useSearchParams } from 'react-router-dom';
import { MapPin, Calendar, Search, ArrowUpDown, ChevronsUpDown } from 'lucide-react';
import { searchFlights } from '../utils/flightUtils';
import { Flight } from '../types';
import FlightCard from '../components/FlightCard';
import Button from '../components/Button';

const FlightsPage: React.FC = () => {
  const [searchParams] = useSearchParams();
  const initialFrom = searchParams.get('from') || '';
  const initialTo = searchParams.get('to') || '';
  const initialDate = searchParams.get('date') || '';

  const [from, setFrom] = useState(initialFrom);
  const [to, setTo] = useState(initialTo);
  const [date, setDate] = useState(initialDate);
  const [flights, setFlights] = useState<Flight[]>([]);
  const [sortBy, setSortBy] = useState<'price' | 'duration' | 'departure'>('price');
  const [sortOrder, setSortOrder] = useState<'asc' | 'desc'>('asc');

  useEffect(() => {
    handleSearch();
  }, [initialFrom, initialTo, initialDate]);

  const handleSearch = () => {
    const foundFlights = searchFlights(from, to, date);
    setFlights(foundFlights);
  };

  const calculateDuration = (flight: Flight) => {
    const departure = new Date(flight.departureTime);
    const arrival = new Date(flight.arrivalTime);
    return arrival.getTime() - departure.getTime();
  };

  const sortFlights = (flights: Flight[]) => {
    return [...flights].sort((a, b) => {
      let comparison = 0;
      
      if (sortBy === 'price') {
        comparison = a.price - b.price;
      } else if (sortBy === 'duration') {
        comparison = calculateDuration(a) - calculateDuration(b);
      } else if (sortBy === 'departure') {
        comparison = new Date(a.departureTime).getTime() - new Date(b.departureTime).getTime();
      }
      
      return sortOrder === 'asc' ? comparison : -comparison;
    });
  };

  const handleSortChange = (newSortBy: 'price' | 'duration' | 'departure') => {
    if (sortBy === newSortBy) {
      setSortOrder(sortOrder === 'asc' ? 'desc' : 'asc');
    } else {
      setSortBy(newSortBy);
      setSortOrder('asc');
    }
  };

  const sortedFlights = sortFlights(flights);

  return (
    <div className="min-h-screen bg-gray-50 pt-24 pb-16">
      <div className="container mx-auto px-4">
        <h1 className="text-3xl font-bold text-[#043927] mb-8">Find Flights</h1>
        
        {/* Search Form */}
        <div className="bg-white p-6 rounded-lg shadow-md mb-8">
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
          
          <Button type="button" onClick={handleSearch} fullWidth>
            <div className="flex items-center justify-center">
              <Search className="mr-2 h-5 w-5" />
              Search Flights
            </div>
          </Button>
        </div>
        
        {/* Sort Options */}
        {flights.length > 0 && (
          <div className="flex flex-wrap gap-4 mb-6">
            <button 
              onClick={() => handleSortChange('price')}
              className={`flex items-center px-4 py-2 rounded-md ${
                sortBy === 'price' 
                  ? 'bg-[#d0e7d2] text-[#043927]' 
                  : 'bg-white text-gray-700'
              }`}
            >
              Price
              {sortBy === 'price' && (
                <ArrowUpDown className={`ml-1 h-4 w-4 ${sortOrder === 'asc' ? 'transform rotate-180' : ''}`} />
              )}
            </button>
            
            <button 
              onClick={() => handleSortChange('duration')}
              className={`flex items-center px-4 py-2 rounded-md ${
                sortBy === 'duration' 
                  ? 'bg-[#d0e7d2] text-[#043927]' 
                  : 'bg-white text-gray-700'
              }`}
            >
              Duration
              {sortBy === 'duration' && (
                <ArrowUpDown className={`ml-1 h-4 w-4 ${sortOrder === 'asc' ? 'transform rotate-180' : ''}`} />
              )}
            </button>
            
            <button 
              onClick={() => handleSortChange('departure')}
              className={`flex items-center px-4 py-2 rounded-md ${
                sortBy === 'departure' 
                  ? 'bg-[#d0e7d2] text-[#043927]' 
                  : 'bg-white text-gray-700'
              }`}
            >
              Departure Time
              {sortBy === 'departure' && (
                <ArrowUpDown className={`ml-1 h-4 w-4 ${sortOrder === 'asc' ? 'transform rotate-180' : ''}`} />
              )}
            </button>
          </div>
        )}
        
        {/* Search Results */}
        {sortedFlights.length > 0 ? (
          <div>
            <h2 className="text-xl font-semibold mb-4">
              {sortedFlights.length} {sortedFlights.length === 1 ? 'flight' : 'flights'} found
            </h2>
            
            <div className="space-y-6">
              {sortedFlights.map((flight) => (
                <FlightCard key={flight.id} flight={flight} />
              ))}
            </div>
          </div>
        ) : (
          <div className="text-center py-12">
            {initialFrom || initialTo || initialDate ? (
              <div>
                <h2 className="text-xl font-semibold mb-2">No flights found</h2>
                <p className="text-gray-600 mb-4">Try adjusting your search criteria or select different dates.</p>
              </div>
            ) : (
              <div>
                <h2 className="text-xl font-semibold mb-2">Search for flights</h2>
                <p className="text-gray-600 mb-4">Enter your travel details to find available flights.</p>
              </div>
            )}
          </div>
        )}
      </div>
    </div>
  );
};

export default FlightsPage;