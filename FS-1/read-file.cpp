#include <iostream>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

int main(int argc, char * argv[])
 {
    if (argc != 2)
    {
        cerr << "Usage: read-file <file_path>\n";
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);

    if (fd == -1)
    {
        cerr << "Error opening\n";
        return 2;
    }

    const ssize_t BUF_SZ = 4096;
    char buf[BUF_SZ];

    while (true)
    {
        ssize_t n = read(fd, buf, BUF_SZ);
        if (n == 0)
        {
            break;
        }
        if (n < 0)
        {
            cerr << "Error reading\n";
            close(fd);
            return 3;
        }

        ssize_t written_total = 0;

        while (written_total < n)
        {
            ssize_t w = write(1, buf + written_total, n - written_total);
            if (w <= 0)
            {
                cerr << "Error writing\n";
                close(fd);
                return 4;
            }
            written_total += w;
        }
    }
    close(fd);
    return 0;
}

