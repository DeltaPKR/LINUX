#include <unistd.h>     
#include <sys/wait.h>   
#include <ctime>        
#include <iostream>     
#include <iomanip>      
#include <cstdio>       

using namespace std;

void do_command(char* argv[])
{
    if (!argv || !argv[0])
    {
        cerr << "no command provided" << endl;
        return;
    }

    time_t t0 = time(nullptr); 

    int pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        execvp(argv[0], argv);
        perror("execvp");
        _exit(127);
    }

    int status = 0;
    if (waitpid(pid, &status, 0) < 0)
    {
        perror("waitpid");
        return;
    }

    time_t t1 = time(nullptr);
    double elapsed = difftime(t1, t0);

    int exit_code = 0;
    if (WIFEXITED(status))
    {
        exit_code = WEXITSTATUS(status);
    }
    else if (WIFSIGNALED(status))
    {
        int sig = WTERMSIG(status);
        cout << "Command terminated by signal " << sig << endl;
        exit_code = 128 + sig;
    }

    cout << "Command completed with " << exit_code << " exit code and took " << fixed << setprecision(0) << elapsed << " seconds" << endl;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        cerr << "usage: " << argv[0] << " <command> [args...]" << endl;
        return 2;
    }

    do_command(argv + 1);
    return 0;
}

