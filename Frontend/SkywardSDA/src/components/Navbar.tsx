import React, { useState, useEffect } from 'react';
import { Link, useNavigate, useLocation } from 'react-router-dom';
import { Plane, Menu, X, LogIn, LogOut, User } from 'lucide-react';
import { isAuthenticated, logoutUser } from '../utils/authUtils';

const Navbar: React.FC = () => {
  const [isScrolled, setIsScrolled] = useState(false);
  const [isMenuOpen, setIsMenuOpen] = useState(false);
  const [isLoggedIn, setIsLoggedIn] = useState(isAuthenticated());
  const navigate = useNavigate();
  const location = useLocation();

  useEffect(() => {
    const handleScroll = () => {
      setIsScrolled(window.scrollY > 10);
    };

    window.addEventListener('scroll', handleScroll);
    return () => window.removeEventListener('scroll', handleScroll);
  }, []);

  useEffect(() => {
    setIsLoggedIn(isAuthenticated());
  }, [location]);

  const handleLogout = () => {
    logoutUser();
    setIsLoggedIn(false);
    navigate('/');
    setIsMenuOpen(false);
  };

  const toggleMenu = () => {
    setIsMenuOpen(!isMenuOpen);
  };

  const closeMenu = () => {
    setIsMenuOpen(false);
  };

  const navbarClasses = `fixed top-0 left-0 right-0 z-50 ${
    isScrolled || location.pathname !== '/'
      ? 'bg-white shadow-md'
      : 'bg-transparent'
  } transition-all duration-300`;

  const linkClasses = `text-${
    isScrolled || location.pathname !== '/' ? '[#043927]' : 'white'
  } font-medium hover:text-[#d0e7d2] transition-colors duration-200`;

  return (
    <nav className={navbarClasses}>
      <div className="container mx-auto px-4 py-4">
        <div className="flex justify-between items-center">
          <Link
            to="/"
            className="flex items-center space-x-2"
            onClick={closeMenu}
          >
            <Plane
              className={`h-8 w-8 ${
                isScrolled || location.pathname !== '/'
                  ? 'text-[#043927]'
                  : 'text-white'
              }`}
            />
            <span
              className={`text-xl font-bold ${
                isScrolled || location.pathname !== '/'
                  ? 'text-[#043927]'
                  : 'text-white'
              }`}
            >
              Skyward
            </span>
          </Link>

          {/* Desktop Navigation */}
          <div className="hidden md:flex items-center space-x-8">
            <Link to="/" className={linkClasses}>
              Home
            </Link>
            <Link to="/flights" className={linkClasses}>
              Flights
            </Link>
            {isLoggedIn ? (
              <>
                <Link to="/dashboard" className={linkClasses}>
                  My Bookings
                </Link>
                <button
                  onClick={handleLogout}
                  className={`${linkClasses} flex items-center gap-1`}
                >
                  <LogOut className="h-4 w-4" />
                  Logout
                </button>
              </>
            ) : (
              <>
                <Link to="/login" className={linkClasses}>
                  <span className="flex items-center gap-1">
                    <LogIn className="h-4 w-4" />
                    Login
                  </span>
                </Link>
                <Link to="/register" className={linkClasses}>
                  <span className="flex items-center gap-1">
                    <User className="h-4 w-4" />
                    Register
                  </span>
                </Link>
              </>
            )}
          </div>

          {/* Mobile Menu Button */}
          <button
            className="md:hidden text-[#043927]"
            onClick={toggleMenu}
            aria-label="Toggle menu"
          >
            {isMenuOpen ? (
              <X className="h-6 w-6" />
            ) : (
              <Menu className="h-6 w-6" />
            )}
          </button>
        </div>

        {/* Mobile Menu */}
        {isMenuOpen && (
          <div className="md:hidden bg-white mt-4 rounded-lg shadow-lg py-4 absolute left-4 right-4 z-50">
            <div className="flex flex-col space-y-4 px-4">
              <Link
                to="/"
                className="text-[#043927] font-medium hover:text-[#d0e7d2] transition-colors"
                onClick={closeMenu}
              >
                Home
              </Link>
              <Link
                to="/flights"
                className="text-[#043927] font-medium hover:text-[#d0e7d2] transition-colors"
                onClick={closeMenu}
              >
                Flights
              </Link>
              {isLoggedIn ? (
                <>
                  <Link
                    to="/dashboard"
                    className="text-[#043927] font-medium hover:text-[#d0e7d2] transition-colors"
                    onClick={closeMenu}
                  >
                    My Bookings
                  </Link>
                  <button
                    onClick={handleLogout}
                    className="text-[#043927] font-medium hover:text-[#d0e7d2] transition-colors text-left flex items-center gap-1"
                  >
                    <LogOut className="h-4 w-4" />
                    Logout
                  </button>
                </>
              ) : (
                <>
                  <Link
                    to="/login"
                    className="text-[#043927] font-medium hover:text-[#d0e7d2] transition-colors flex items-center gap-1"
                    onClick={closeMenu}
                  >
                    <LogIn className="h-4 w-4" />
                    Login
                  </Link>
                  <Link
                    to="/register"
                    className="text-[#043927] font-medium hover:text-[#d0e7d2] transition-colors flex items-center gap-1"
                    onClick={closeMenu}
                  >
                    <User className="h-4 w-4" />
                    Register
                  </Link>
                </>
              )}
            </div>
          </div>
        )}
      </div>
    </nav>
  );
};

export default Navbar;