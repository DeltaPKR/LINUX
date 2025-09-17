#include "sort_complex.h"
#include <algorithm>

using namespace std;

static bool cmpMag( const Complex & a , const Complex & b )
{
	return a.abs() < b.abs();
}


void sortByMagnitude(vector<Complex> &v)
{
	sort( v.begin() , v.end() , cmpMag );
}
