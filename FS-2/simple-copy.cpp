#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

using namespace std;

int main(int argc, char * argv[])
 {
    if (argc != 3)
    {
        cerr << "Usage: simple-copy <source> <destination>\n";
        return 1;
    }

    int src = open(argv[1], O_RDONLY);
    if (src == -1)
    {
        cerr << "Error opening source: " << argv[1] << " : " << strerror(errno) << '\n';
        return 2;
    }

    int dst = open(argv[2], O_WRONLY);
    if (dst == -1)
    {
        cerr << "Error opening destination: " << argv[2] << " : " << strerror(errno) << '\n';
        close(src);
        return 3;
    }

    const size_t BUF = 8192;
    char buf[BUF];

    while (true)
    {
        ssize_t r = read(src, buf, BUF);
        if (r == 0)
        {
            break;
        }
        if (r < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            cerr << "Error reading source: " << strerror(errno) << '\n';
            close(src);
            close(dst);
            return 4;
        }

        ssize_t to_write = r;
        ssize_t written_total = 0;

        while (written_total < to_write)
        {
            ssize_t w = write(dst, buf + written_total, (size_t) (to_write - written_total));
            if (w < 0)
            {
                if (errno == EINTR)
                {
                    continue;
                }
                cerr << "Error writing destination: " << strerror(errno) << '\n';
                close(src);
                close(dst);
                return 5;
            }
            if (w == 0)
            {
                cerr << "Write returned 0\n";
                close(src);
                close(dst);
                return 6;
            }
            written_total += w;
        }
    }

    if (close(src) < 0)
    {
        cerr << "Error closing source: " << strerror(errno) << '\n';
    }
    if (close(dst) < 0)
    {
        cerr << "Error closing destination: " << strerror(errno) << '\n';
    }

    return 0;
}

