#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cerrno>
#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        cerr << "usage: " << argv[0] << " <source> <destination>" << endl;
        return 1;
    }

    const char* src_path = argv[1];
    const char* dst_path = argv[2];

    int src = open(src_path, O_RDONLY);
    if (src < 0)
    {
        perror("open source");
        return 2;
    }

    struct stat st;
    if (fstat(src, &st) != 0)
    {
	perror("fstat source");
	close(src);
	return 2;
    }

    if (!S_ISREG(st.st_mode))
    {
        cerr << "source is not a regular file" << endl;
        close(src);
        return 3;
    }

    int size = st.st_size;

    int dst = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst < 0)
    {
        perror("open dest");
        close(src);
        return 4;
    }

    const int BUF = 64 * 1024;
    char* buf = (char*)malloc(BUF);
    if (!buf)
    {
        perror("malloc");
        close(src);
        close(dst);
        return 5;
    }

    int offset = 0;
    int data_copied = 0;
    bool seek_supported = true;

    int test = lseek(src, 0, SEEK_DATA);
    if (test == -1)
        seek_supported = false;
    lseek(src, 0, SEEK_SET);

    if (!seek_supported)
    {
        int total = 0;
        while (true)
        {
            int r = read(src, buf, BUF);
            if (r < 0)
            {
                if (errno == EINTR) continue;
                perror("read");
                free(buf);
                close(src);
                close(dst);
                return 6;
            }
            if (r == 0) break;
            int wptr = 0;
            while (wptr < r)
            {
                int w = write(dst, buf + wptr, r - wptr);
                if (w < 0)
                {
                    if (errno == EINTR) continue;
                    perror("write");
                    free(buf);
                    close(src);
                    close(dst);
                    return 7;
                }
                wptr += w;
            }
            total += r;
        }
        cout << "Successfully copied " << total << " bytes (data: " << total << ", hole: 0)" << endl;
        free(buf);
        close(src);
        close(dst);
        return 0;
    }

    while (offset < size)
    {
        int data_off = lseek(src, offset, SEEK_DATA);
        if (data_off == -1)
        {
            if (errno == ENXIO) break;
            perror("lseek SEEK_DATA");
            free(buf);
            close(src);
            close(dst);
            return 8;
        }

        if (data_off > offset)
        {
            if (lseek(dst, data_off, SEEK_SET) == -1)
            {
                perror("lseek dest");
                free(buf);
                close(src);
                close(dst);
                return 9;
            }
        }

        int data_end = lseek(src, data_off, SEEK_HOLE);
        if (data_end == -1)
        {
            perror("lseek SEEK_HOLE");
            free(buf);
            close(src);
            close(dst);
            return 10;
        }

        int to_copy = data_end - data_off;
        if (lseek(src, data_off, SEEK_SET) == -1)
        {
            perror("lseek src set");
            free(buf);
            close(src);
            close(dst);
            return 11;
        }

        while (to_copy > 0)
        {
            int r = read(src, buf, (int)(to_copy > (int)BUF ? BUF : (int)to_copy));
            if (r < 0)
            {
                if (errno == EINTR) continue;
                perror("read");
                free(buf);
                close(src);
                close(dst);
                return 12;
            }
            if (r == 0) break;
            int wptr = 0;
            while (wptr < r)
            {
                int w = write(dst, buf + wptr, r - wptr);
                if (w < 0)
                {
                    if (errno == EINTR) continue;
                    perror("write");
                    free(buf);
                    close(src);
                    close(dst);
                    return 13;
                }
                wptr += w;
                data_copied += w;
            }
            to_copy -= r;
        }

        offset = data_end;
    }

    if (size > 0)
    {
        if (ftruncate(dst, size) != 0)
        {
            perror("ftruncate");
            free(buf);
            close(src);
            close(dst);
            return 14;
        }
    }

    free(buf);
    close(src);
    close(dst);

    int hole = size - data_copied;
    cout << "Successfully copied " << size << " bytes (data: " << data_copied << ", hole: " << hole << ")" << endl;

    return 0;
}
