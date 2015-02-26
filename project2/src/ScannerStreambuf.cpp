#include "ScannerStreambuf.h"

ScannerStreambuf::ScannerStreambuf(std::istream *in)
:
	d_in(in)
{
	setg(0, 0, 0);
}
    
ScannerStreambuf::ScannerStreambuf(std::string const &infile) :
	d_in(nullptr)
{
	if (d_fb.open(infile, std::ios::in))
	{
		d_in = new std::istream(&d_fb);
		setg(0, 0, 0);
	}
}
    
ScannerStreambuf::~ScannerStreambuf()
{
	d_fb.close();
}
    
int ScannerStreambuf::underflow()
{
	if (d_column == d_line.length())
	{
		if (!getline(*d_in, d_line))
			return EOF;

		d_column = 0;
		d_line += '\n';
	}
	d_ch = d_line[d_column++];
	setg(&d_ch, &d_ch, &d_ch + 1);
	return *gptr();
}
      
