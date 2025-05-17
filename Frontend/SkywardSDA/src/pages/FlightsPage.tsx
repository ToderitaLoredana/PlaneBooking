// src/pages/FlightsPage.tsx
import React, { useState, useEffect } from 'react';
import { useSearchParams } from 'react-router-dom';
import { searchFlights } from '../services/apiClient';

interface Segment {
  from: string;
  to: string;
  day: string;
  departure_time: string;
  arrival_time: string;
  duration: number;
  cost: number;
  distance: number;
}

interface Journey {
  total_cost: number;
  total_duration: number;
  segments: Segment[];
}

interface ApiResponse {
  origin: string;
  destination: string;
  departure_day: string;
  departure_time: string;
  journeys: {
    cheapest: Journey;
    fastest: Journey;
    optimal: Journey;
  };
}

const FlightsPage: React.FC = () => {
  const [searchParams] = useSearchParams();
  const origin = searchParams.get('from') || '';
  const destination = searchParams.get('to') || '';
  const date = searchParams.get('date') || '';
  const time = searchParams.get('time') || '';

  const [data, setData] = useState<ApiResponse | null>(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    if (origin && destination && date && time) {
      fetchJourneys();
    }
  }, [origin, destination, date, time]);

  const fetchJourneys = async () => {
    setLoading(true);
    setError(null);
    try {
      const resp = (await searchFlights({
        source: origin.toUpperCase(),
        destination: destination.toUpperCase(),
        day: new Date(date)
          .toLocaleDateString('en-US', { weekday: 'long' })
          .toLowerCase(),
        departure_time: time,
      })) as ApiResponse;
      setData(resp);
    } catch (e) {
      console.error(e);
      setError('Failed to fetch flight options.');
    } finally {
      setLoading(false);
    }
  };

  const formatDuration = (mins: number) => {
    const h = Math.floor(mins / 60);
    const m = mins % 60;
    return `${h}h ${m}m`;
  };

  const renderJourney = (label: string, j: Journey) => (
    <div key={label} className="bg-white p-6 rounded-lg shadow-md mb-6">
      <h3 className="text-xl font-bold mb-2 capitalize text-[#043927]">
        {label} journey
      </h3>
      <div className="flex gap-8 mb-4">
        <div><strong>Total Cost:</strong> ${j.total_cost}</div>
        <div><strong>Total Duration:</strong> {formatDuration(j.total_duration)}</div>
      </div>
      <div className="overflow-x-auto">
        <table className="min-w-full text-sm text-left">
          <thead>
            <tr className="bg-gray-100">
              {['From','To','Day','Depart','Arrive','Duration','Cost','Distance']
                .map(h => <th key={h} className="px-3 py-2">{h}</th>)}
            </tr>
          </thead>
          <tbody>
            {j.segments.map((s, i) => (
              <tr key={i} className="border-b">
                <td className="px-3 py-2">{s.from}</td>
                <td className="px-3 py-2">{s.to}</td>
                <td className="px-3 py-2">{s.day}</td>
                <td className="px-3 py-2">{s.departure_time}</td>
                <td className="px-3 py-2">{s.arrival_time}</td>
                <td className="px-3 py-2">{formatDuration(s.duration)}</td>
                <td className="px-3 py-2">${s.cost}</td>
                <td className="px-3 py-2">{s.distance} km</td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>
    </div>
  );

  return (
    <div className="min-h-screen bg-gray-50 pt-24 pb-16">
      <div className="container mx-auto px-4">
        <h1 className="text-3xl font-bold text-[#043927] mb-8">Flight Results</h1>

        {loading && <p className="py-12 text-center">Loading…</p>}
        {error && <p className="text-red-500 mb-6">{error}</p>}

        {data && !loading && (
          <>
            {/* Search Summary */}
            <div className="bg-white p-6 rounded-lg shadow-md mb-8">
              <h2 className="text-2xl font-bold text-[#043927] mb-4">
                Search Summary
              </h2>
              <p><strong>Origin:</strong> {data.origin}</p>
              <p><strong>Destination:</strong> {data.destination}</p>
              <p>
                <strong>Departure Day:</strong>{' '}
                {data.departure_day.charAt(0).toUpperCase() + data.departure_day.slice(1)}
              </p>
              <p><strong>Departure Time:</strong> {data.departure_time}</p>
            </div>

            {/* Itineraries */}
            {renderJourney('cheapest', data.journeys.cheapest)}
            {renderJourney('fastest', data.journeys.fastest)}
            {renderJourney('optimal', data.journeys.optimal)}
          </>
        )}
      </div>
    </div>
  );
};

export default FlightsPage;
