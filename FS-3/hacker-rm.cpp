#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>

using namespace std;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cout << "usage: " << argv[0] << " <file>";
        return 1;
    }

    const char* path = argv[1];
    struct stat st;

    if (stat(path, &st) != 0)
    {
        cout << "stat failed";
        return 2;
    }

    if (!S_ISREG(st.st_mode))
    {
        cout << "not a regular file";
        return 3;
    }

    int fd = open(path, O_WRONLY);
    if (fd < 0)
    {
        cout << "open failed";
        return 4;
    }

    int remaining = (int)st.st_size;
    const int BUF = 8192;
    char zero[BUF];
    memset(zero, 0, BUF);

    while (remaining > 0)
    {
        int to_write = remaining > BUF ? BUF : remaining;
        int written = write(fd, zero, to_write);

        if (written < 0)
        {
            cout << "write failed";
            close(fd);
            return 5;
        }
        remaining -= written;
    }

    fsync(fd);
    close(fd);

    if (unlink(path) != 0)
    {
        perror("unlink");
        return 6;
    }

    return 0;
}
