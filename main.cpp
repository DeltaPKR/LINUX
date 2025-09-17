#include <iostream>
#include <vector>
#include "Complex.h"
#include "sort_complex.h"

using namespace std;

int main()
{
	vector<Complex> nums{
		{ 3.0 , 4.0 } ,
		{ 1.0 , 1.0 } ,
		{ 0.0 , 2.0 } ,
		{ -2.0 , -2.0 } ,
		{ 5.0 , 12.0 }};

	cout << "Before" << endl;

	for ( const auto & c : nums )
	{
		cout << "(" << c.real() << "," << c.imag() << ") abs=" << c.abs() << "";
	}

	sortByMagnitude(nums);
	cout << endl;
	cout << "After" << endl;

	for ( const auto & c : nums )
	{
		cout << "(" << c.real() << "," << c.imag() << ") abs=" << c.abs() << "";
	}

	return 0;
}
