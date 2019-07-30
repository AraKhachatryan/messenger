/******************************************************************************
*  @file namespace_terminal.hpp
*  @brief namespace terminal designed for manipulating with linux terminal
*
*  @example std::cout << terminal::TEXTCOLOR_BLUE after this all typed in the
*       terminal will be blue
*  @example std::cout << terminal::TEXT_BOLD after this all typed in the
*       terminal will be bold
*  @example std::cout << terminal::RESET_ALL resets all to the default mode
*  @example std::cout << terminal::Cursor_Position(0, 0) Moves the cursor to
*       row 0, column 0 (to the beginning of terminal)
*..............................................................................
*  @brief Used ANSI control codes and escape sequences
*    @see http://en.wikipedia.org/wiki/ANSI_escape_code
*    @see http://ascii-table.com/ansi-escape-sequences.php
*  @version 3.1.0
*  @author Ara Khachatryan
******************************************************************************/

#ifndef NAMESPACE_TERMINAL_HPP
#define NAMESPACE_TERMINAL_HPP

#include <ostream>
#include <sstream>
#include <string>


// enable or disable color printing in linux terminal
extern bool terminal_color;



namespace terminal {

	// Sets SGR(Select Graphic Rendition) parameters, including text colors
	enum SGR {
		RESET_ALL          = 0,
		TEXT_BOLD          = 1,
		TEXTCOLOR_BLACK    = 30,
		TEXTCOLOR_RED      = 31,
		TEXTCOLOR_GREEN    = 32,
		TEXTCOLOR_YELLOW   = 33,
		TEXTCOLOR_BLUE     = 34,
		TEXTCOLOR_MAGENDA  = 35,
		TEXTCOLOR_CYAN     = 36,
		TEXTCOLOR_WHITE    = 37,
		TEXTCOLOR_DEFAULT  = 39,
	};


	// operator<< overloaded
	inline std::ostream& operator<<( std::ostream& stream, SGR code )
	{
		if ( ::terminal_color ) {
			return stream << "\x1B[" << static_cast<int>(code) << "m";
		} else {
			return stream;
		}
	}


	// Moves cursor to beginning of the line n (default 1) lines up
	inline std::string Cursor_Previous_Line( int n = 1 )
	{
		if ( ::terminal_color ) {
			std::ostringstream stringStream;
			stringStream << "\x1B[" << n << "F";
			return stringStream.str();
		} else {
			return "";
		}
	}


	// Moves the cursor to column n
	inline std::string Cursor_Horizontal_Absolute( int n )
	{
		if ( ::terminal_color ) {
			std::ostringstream stringStream;
			stringStream << "\x1B[" << n << "G";
			return stringStream.str();
		} else {
			return "";
		}
	}


	// Moves the cursor to row n, column m
	inline std::string Cursor_Position( int n, int m )
	{
		if ( ::terminal_color ) {
			std::ostringstream stringStream;
			stringStream << "\x1B[" << n << ";" << m << "H";
			return stringStream.str();
		} else {
			return "";
		}
	}


	// Clears part of the screen. If n is 0 (or missing), clear from cursor to
	// end of screen. If n is 1, clear from cursor to beginning of the screen.
	// If n is 2, clear entire screen (and moves cursor to upper left)
	inline std::string Erase_Display( int n = 2 )
	{
		if ( ::terminal_color ) {
			std::ostringstream stringStream;
			stringStream << "\x1B[" << n << "J";
			return stringStream.str();
		} else {
			return "";
		}
	}


    // Clears part of the Line. If n is 0 (or missing), clear from cursor to the
	// end of the line. If n is 1, clear from cursor to beginning of the line.
	// If n is 2, clear entire line. Cursor position does not change.
	inline std::string Erase_Line( int n = 2 )
	{
		if ( ::terminal_color ) {
			std::ostringstream stringStream;
			stringStream << "\x1B[" << n << "K";
			return stringStream.str();
		} else {
			return "";
		}
	}


	// Scroll whole page up by n (default 1) lines.
	// New lines are added at the bottom
	inline std::string Scroll_Up( int n = 1 )
	{
		if ( ::terminal_color ) {
			std::ostringstream stringStream;
			stringStream << "\x1B[" << n << "S";
			return stringStream.str();
		} else {
			return "";
		}
	}


	// Scroll whole page down by n (default 1) lines.
	// New lines are added at the top
	inline std::string Scroll_Down( int n = 1 )
	{
		if ( terminal_color ) {
			std::ostringstream stringStream;
			stringStream << "\x1B[" << n << "T";
			return stringStream.str();
		} else {
			return "";
		}
	}


	// Saves the cursor position
	inline std::ostream& Save_Cursor_Position( std::ostream& stream )
	{
		if( ::terminal_color ) {
			return stream << "\x1B[s";
		} else {
			return stream;
		}
	}


	// Restores the cursor position
	inline std::ostream& Restore_Cursor_Position( std::ostream& stream )
	{
		if ( ::terminal_color ) {
			return stream << "\x1B[u";
		} else {
			return stream;
		}
	}


}

#endif // NAMESPACE_TERMINAL_HPP

