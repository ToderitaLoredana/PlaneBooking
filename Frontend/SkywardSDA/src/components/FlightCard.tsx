import React from 'react';
import { Flight } from '../types';
import { Plane } from 'lucide-react';
import Button from './Button';
import { useNavigate } from 'react-router-dom';

interface FlightCardProps {
  flight: Flight;
}

const FlightCard: React.FC<FlightCardProps> = ({ flight }) => {
  const navigate = useNavigate();
  
  // Format date and time
  const formatDateTime = (dateTimeString: string) => {
    const date = new Date(dateTimeString);
    return new Intl.DateTimeFormat('en-US', {
      weekday: 'short',
      month: 'short', 
      day: 'numeric',
      hour: '2-digit',
      minute: '2-digit',
    }).format(date);
  };
  
  // Calculate flight duration
  const calculateDuration = () => {
    const departure = new Date(flight.departureTime);
    const arrival = new Date(flight.arrivalTime);
    const durationMs = arrival.getTime() - departure.getTime();
    
    const hours = Math.floor(durationMs / (1000 * 60 * 60));
    const minutes = Math.floor((durationMs % (1000 * 60 * 60)) / (1000 * 60));
    
    return `${hours}h ${minutes}m`;
  };
  
  const handleSelectFlight = () => {
    navigate(`/booking/${flight.id}`);
  };
  
  return (
    <div className="bg-white rounded-lg shadow-md hover:shadow-xl transition-shadow duration-300 p-6 mb-6 border border-gray-100">
      <div className="flex flex-col lg:flex-row justify-between gap-4">
        <div className="flex items-center">
          <div className="p-3 rounded-full bg-[#d0e7d2] mr-4">
            <Plane className="text-[#043927] h-6 w-6" />
          </div>
          <div>
            <h3 className="font-bold text-lg">{flight.airline}</h3>
            <p className="text-gray-500 text-sm">Flight: {flight.flightNumber}</p>
          </div>
        </div>
        
        <div className="flex flex-col md:flex-row gap-6 lg:gap-10 py-4">
          <div className="text-center">
            <p className="font-bold text-xl">{formatDateTime(flight.departureTime).split(',')[1]}</p>
            <p className="text-gray-600">{flight.departureCity}</p>
          </div>
          
          <div className="flex flex-col items-center justify-center">
            <div className="relative w-24 md:w-32 lg:w-40">
              <div className="absolute top-1/2 w-full h-0.5 bg-gray-300"></div>
              <div className="absolute left-0 top-1/2 transform -translate-y-1/2 w-2 h-2 rounded-full bg-[#043927]"></div>
              <div className="absolute right-0 top-1/2 transform -translate-y-1/2 w-2 h-2 rounded-full bg-[#043927]"></div>
            </div>
            <p className="text-sm text-gray-500 mt-1">{calculateDuration()}</p>
          </div>
          
          <div className="text-center">
            <p className="font-bold text-xl">{formatDateTime(flight.arrivalTime).split(',')[1]}</p>
            <p className="text-gray-600">{flight.destinationCity}</p>
          </div>
        </div>
        
        <div className="flex flex-col justify-between items-end">
          <p className="text-2xl font-bold text-[#043927]">${flight.price}</p>
          <Button onClick={handleSelectFlight} variant="primary">
            Select Flight
          </Button>
        </div>
      </div>
    </div>
  );
};

export default FlightCard;