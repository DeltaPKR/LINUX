#include <iostream>
#include <stdexcept>

using namespace std;

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        cout << "Usage: " << argv[0] << " a b c" << endl;
        return 1;
    }

    try
    {
        int a = stoi(argv[1]);
        int b = stoi(argv[2]);
        int c = stoi(argv[3]);

        int mn = a;
        if (b < mn) mn = b;
        if (c < mn) mn = c;

        int mx = a;
        if (b > mx) mx = b;
        if (c > mx) mx = c;

        cout << "min-" << mn << ", max-" << mx << endl;
    }
    catch (invalid_argument&)
    {
        cout << "Error: one of the arguments is not a number" << endl;
        return 1;
    }
    catch (out_of_range&)
    {
        cout << "Error: one of the arguments is too large for int" << endl;
        return 1;
    }

    return 0;
}
