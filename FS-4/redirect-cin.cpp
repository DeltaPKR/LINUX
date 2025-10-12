#include <iostream>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>

using namespace std;

void initialize(int argc, char* argv[])
{
    if (argc != 2)
    {
        cout << "usage: " << argv[0] << " <file>" << endl;
        exit(1);
    }

    const char* path = argv[1];
    int fd = open(path, O_RDONLY);

    if (fd < 0)
    {
        perror("open");
        exit(4);
    }

    if (dup2(fd, 0) < 0)
    {
        perror("dup2");
        close(fd);
        exit(5);
    }
    close(fd);
}

int main(int argc, char** argv)
{
    initialize(argc, argv);

    string input;
    cin >> input;

    reverse(input.begin(), input.end());

    cout << input << endl;

    return 0;
}
