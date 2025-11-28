#include <iostream>
#include <unistd.h>
#include <pwd.h>
#include <signal.h>
#include <ucontext.h>
#include <cstring>

void handler(int, siginfo_t *info, void *context)
{
	pid_t spid = info->si_pid;
	uid_t suid = info->si_uid;
	passwd *pw = getpwuid(suid);
	const char *uname = pw ? pw->pw_name : "unknown";

#if defined(__x86_64__)
	ucontext_t *uc = static_cast<ucontext_t*>(context);
	unsigned long rip = uc->uc_mcontext.gregs[REG_RIP];
	unsigned long rax = uc->uc_mcontext.gregs[REG_RAX];
	unsigned long rbx = uc->uc_mcontext.gregs[REG_RBX];

	unsigned int eip = rip;
        unsigned int eax = rax;
        unsigned int ebx = rbx;

	std::cout << "Received a SIGUSR1 signal from process "
              << spid << " executed by "
              << suid << " (" << uname << ")" << std::endl;

    std::cout << "State of the context: EIP = 0x"
              << std::hex << eip
              << ", EAX = 0x" << eax
              << ", EBX = 0x" << ebx
              << std::dec << std::endl;

#elif defined(__i386__)
    ucontext_t *uc = static_cast<ucontext_t *>(context);
    unsigned long eip = uc->uc_mcontext.gregs[REG_EIP];
    unsigned long eax = uc->uc_mcontext.gregs[REG_EAX];
    unsigned long ebx = uc->uc_mcontext.gregs[REG_EBX];

    std::cout << "Received a SIGUSR1 signal from process "
              << spid << " executed by "
              << suid << " (" << uname << ")" << std::endl;

    std::cout << "State of the context: EIP = 0x"
              << std::hex << eip
              << ", EAX = 0x" << eax
              << ", EBX = 0x" << ebx
              << std::dec << std::endl;
#else
    std::cout << "Received a SIGUSR1 signal from process "
              << spid << " executed by "
              << suid << " (" << uname << ")" << std::endl;

    std::cout << "State of the context: unsupported architecture." << std::endl;
#endif
}

int main()
{
    std::cout << "PID: " << getpid() << "\n";

    struct sigaction sa;
    std::memset(&sa, 0, sizeof(sa));

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler;

    sigaction(SIGUSR1, &sa, nullptr);

    for (;;)
    {
        sleep(10);
    }
}
