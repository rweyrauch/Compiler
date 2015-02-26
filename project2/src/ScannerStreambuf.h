#pragma once
#include <iostream>
#include <string>
#include <fstream>

class ScannerStreambuf: public std::streambuf
{
	std::istream* d_in;
	std::string d_line;
	std::filebuf d_fb;
	size_t d_column = 0;
	char d_ch;

	public:
		ScannerStreambuf(std::istream *in);
		ScannerStreambuf(std::string const &infile);
		~ScannerStreambuf();
		
		std::string const &line() const { return d_line; }
		size_t column() const { return d_column; }

	private:
		int underflow() override;    
};
   
