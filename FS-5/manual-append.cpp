#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

using namespace std;

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		cout << "usage: " << argv[0] << " result file" << endl;
	}

	const char* path = argv[1];
	int fd1 = open(path, O_WRONLY | O_CREAT, 0644);

	if(fd1 < 0)
	{
		perror("open");
		return 1;
	}
	
	int fd2 = dup(fd1);

	if(fd2 < 0)
	{
		perror("open2");
		close(fd1);
		return 2;
	}

	const char* first = "first line\n";
	const char* second = "second line\n";

	ssize_t n = write(fd1, first, strlen(first));

	if (n < 0)
	{
		perror("write first");
		close(fd1);
		close(fd2);
		return 3;
	}

	n = write(fd2, second, strlen(second));

	if (n < 0)
	{
		perror("write second");
		close(fd1);
		close(fd2);
		return 4;
	}

	close(fd1);
	close(fd2);
	return 0;
}
