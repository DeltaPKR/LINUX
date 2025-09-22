#include <iostream>
#include <string>
#include <stdexcept>

using namespace std;

bool isInt(const string& s)
{
    if (s.empty()) return false;
    int start = 0;
    if (s[0] == '-' || s[1] == '+') start = 1;
    if (start == s.size()) return false;

    for (int i = start; i < s.size(); i++)
    {
        if (!isdigit(s[i])) return false;
    }
    return true;
}

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        cout << "Usage: " << argv[0] << " a b c" << endl;
        return 1;
    }

    try
    {
        string a = argv[1];
        string b = argv[2];
        string c = argv[3];

        if (!isInt(a) || !isInt(b) || !isInt(c))
        {
            cout << "Error: all arguments must be integers" << endl;
            return 1;
        }

        int ai = stoi(a);
        int bi = stoi(b);
        int ci = stoi(c);

        int mn = ai;
        if (bi < mn) mn = bi;
        if (ci < mn) mn = ci;

        int mx = ai;
        if (bi > mx) mx = bi;
        if (ci > mx) mx = ci;

        cout << "min-" << mn << ", max-" << mx << endl;
    }
    catch (out_of_range&)
    {
        cout << "Error: one of the arguments is too large for int" << endl;
        return 1;
    }

    return 0;
}
