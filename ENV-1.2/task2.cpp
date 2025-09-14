#include <iostream>
#include <stdexcept>

using namespace std;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cout << "Usage: " << argv[0] << " arg1 arg2 ..." << endl;
        return 1;
    }

    try
    {
        for (int i = argc - 1; i > 0; i--)
        {
            string s = argv[i];
            cout << s << endl;
        }
    }
    catch (exception& e)
    {
        cout << "Unexpected error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
