#ifndef COMPLEX_H
#define COMPLEX_H


class Complex
{
public:
	Complex( double re = 0.0 , double im = 0.0 );
	double real() const;
	double imag() const;
	Complex operator + ( const Complex & other ) const;
	Complex operator - ( const Complex & other ) const;
	Complex operator * ( double scalar ) const;
	double abs() const;
private:
	double re;
	double im;
};

#endif
