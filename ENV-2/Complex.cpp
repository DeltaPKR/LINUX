#include "Complex.h"
#include <cmath>

Complex::Complex( double re_ , double im_ ) : re{ re_ } , im{ im_ }{}

double Complex::real() const
{
	return re;
}

double Complex::imag() const
{
	return im;
}

Complex Complex::operator + ( const Complex & other ) const
{
	return Complex{ re + other.re , im + other.im };
}
Complex Complex::operator - ( const Complex & other ) const
{
	return Complex{ re - other.re , im - other.im };
}
Complex Complex::operator * ( double scalar ) const
{
	return Complex{ re * scalar , im * scalar };
}

double Complex::abs() const
{
	return std::sqrt( re * re + im * im );
}
