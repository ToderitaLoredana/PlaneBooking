import React from 'react';
import { Link } from 'react-router-dom';
import { Plane, Phone, Mail, MapPin, Facebook, Twitter, Instagram, Youtube } from 'lucide-react';

const Footer: React.FC = () => {
  return (
    <footer className="bg-[#043927] text-white pt-12 pb-6">
      <div className="container mx-auto px-4">
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-8">
          <div>
            <div className="flex items-center mb-4">
              <Plane className="h-6 w-6 mr-2" />
              <h3 className="text-xl font-bold">Skyward</h3>
            </div>
            <p className="text-gray-300 mb-4">
              Book your flights with confidence. Travel the world with Skyward's exceptional service and competitive prices.
            </p>
            <div className="flex space-x-4">
              <a href="#" className="text-white hover:text-[#d0e7d2] transition-colors">
                <Facebook className="h-5 w-5" />
              </a>
              <a href="#" className="text-white hover:text-[#d0e7d2] transition-colors">
                <Twitter className="h-5 w-5" />
              </a>
              <a href="#" className="text-white hover:text-[#d0e7d2] transition-colors">
                <Instagram className="h-5 w-5" />
              </a>
              <a href="#" className="text-white hover:text-[#d0e7d2] transition-colors">
                <Youtube className="h-5 w-5" />
              </a>
            </div>
          </div>
          
          <div>
            <h3 className="text-lg font-semibold mb-4">Quick Links</h3>
            <ul className="space-y-2">
              <li>
                <Link to="/" className="text-gray-300 hover:text-[#d0e7d2] transition-colors">Home</Link>
              </li>
              <li>
                <Link to="/flights" className="text-gray-300 hover:text-[#d0e7d2] transition-colors">Find Flights</Link>
              </li>
              <li>
                <Link to="/login" className="text-gray-300 hover:text-[#d0e7d2] transition-colors">Login</Link>
              </li>
              <li>
                <Link to="/register" className="text-gray-300 hover:text-[#d0e7d2] transition-colors">Register</Link>
              </li>
            </ul>
          </div>
          
          <div>
            <h3 className="text-lg font-semibold mb-4">Information</h3>
            <ul className="space-y-2">
              <li>
                <a href="#" className="text-gray-300 hover:text-[#d0e7d2] transition-colors">About Us</a>
              </li>
              <li>
                <a href="#" className="text-gray-300 hover:text-[#d0e7d2] transition-colors">Travel Guide</a>
              </li>
              <li>
                <a href="#" className="text-gray-300 hover:text-[#d0e7d2] transition-colors">FAQs</a>
              </li>
              <li>
                <a href="#" className="text-gray-300 hover:text-[#d0e7d2] transition-colors">Terms & Conditions</a>
              </li>
              <li>
                <a href="#" className="text-gray-300 hover:text-[#d0e7d2] transition-colors">Privacy Policy</a>
              </li>
            </ul>
          </div>
          
          <div>
            <h3 className="text-lg font-semibold mb-4">Contact Us</h3>
            <ul className="space-y-3">
              <li className="flex items-start">
                <MapPin className="h-5 w-5 mr-2 text-[#d0e7d2] mt-1" />
                <span className="text-gray-300">123 Skyward Tower, Aviation Blvd, New York, NY 10001</span>
              </li>
              <li className="flex items-center">
                <Phone className="h-5 w-5 mr-2 text-[#d0e7d2]" />
                <span className="text-gray-300">+1 (555) 123-4567</span>
              </li>
              <li className="flex items-center">
                <Mail className="h-5 w-5 mr-2 text-[#d0e7d2]" />
                <span className="text-gray-300">info@skyward-airlines.com</span>
              </li>
            </ul>
          </div>
        </div>
        
        <div className="border-t border-gray-700 mt-8 pt-6">
          <p className="text-center text-gray-400 text-sm">
            &copy; {new Date().getFullYear()} Skyward Airlines. All rights reserved.
          </p>
        </div>
      </div>
    </footer>
  );
};

export default Footer;