#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <limits.h>

std::string trim(const std::string &s)
 {
     size_t a = 0;
     while (a < s.size() && (s[a] == ' ' || s[a] == '\t')) ++a;
     size_t b = s.size();
     while (b > a && (s[b - 1] == ' ' || s[b - 1] == '\t')) --b;
     return s.substr(a, b - a);
 }

std::vector<std::string> split(const std::string& str, char d)
 {
     std::vector<std::string> res;
     std::stringstream ss(str);
     std::string token;
     while (std::getline(ss, token, d))
     {
         if (!token.empty())
         {
             res.push_back(token);
         }
     }
     return res;
 }

std::vector<std::string> split_once(const std::string& str, const std::string& delim)
 {
     std::vector<std::string> res;
     size_t pos = str.find(delim);
     if (pos == std::string::npos)
     {
         res.push_back(str);
     }
     else
     {
         res.push_back(str.substr(0, pos));
         res.push_back(str.substr(pos + delim.length()));
     }
     return res;
 }

int run_command(const std::string &cmd, int output_fd = 1, bool silent = false)
 {
     std::vector<std::string> args = split(cmd, ' ');
     if (args.empty()) return 0;
     int pid = fork();
     if (pid == 0)
     {
         char cwd[PATH_MAX];
         if (getcwd(cwd, sizeof cwd) != nullptr)
         {
             const char *old = getenv("PATH");
             std::string newp;
             if (old && old[0])
             {
                 newp = std::string(cwd) + ":" + old;
             }
             else
             {
                 newp = std::string(cwd);
             }
             setenv("PATH", newp.c_str(), 1);
         }

         if (output_fd >= 0 && output_fd != 1)
         {
             dup2(output_fd, 1);
             dup2(output_fd, 2);
             close(output_fd);
         }
         else if (output_fd == 1 && silent)
         {
             pid_t me = getpid();
             char fname[64];
             snprintf(fname, sizeof fname, "%d.log", (int)me);
             int fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
             if (fd == -1)
             {
                 std::cerr << "open failed for " << fname << " : " << std::strerror(errno) << std::endl;
                 _exit(127);
             }
             dup2(fd, 1);
             dup2(fd, 2);
             close(fd);
         }
         std::vector<char*> argv;
         argv.reserve(args.size() + 1);
         for (auto &a : args)
         {
             argv.push_back(const_cast<char*>(a.c_str()));
         }
         argv.push_back(nullptr);
         execvp(argv[0], argv.data());
         std::cerr << "execvp failed for " << (argv[0] ? argv[0] : "unknown") << " : " << std::strerror(errno) << std::endl;
         _exit(EXIT_FAILURE);
     }
     else if (pid > 0)
     {
         int status;
         if (waitpid(pid, &status, 0) == -1)
         {
             std::cerr << "waitpid failed for: " << cmd << " : " << std::strerror(errno) << std::endl;
             return 1;
         }
         if (WIFEXITED(status))
         {
             return WEXITSTATUS(status);
         }
         return 1;
     }
     else
     {
         std::cerr << "fork failed for: " << cmd << " : " << std::strerror(errno) << std::endl;
         return 1;
     }
 }

int eval_command(const std::string &command_input)
 {
     std::string cmd = trim(command_input);
     if (cmd.empty()) return 0;

     if (cmd.find("&&") != std::string::npos)
     {
         auto parts = split_once(cmd, "&&");
         int left = eval_command(trim(parts[0]));
         if (left == 0)
         {
             return eval_command(trim(parts[1]));
         }
         return left;
     }
     else if (cmd.find("||") != std::string::npos)
     {
         auto parts = split_once(cmd, "||");
         int left = eval_command(trim(parts[0]));
         if (left != 0)
         {
             return eval_command(trim(parts[1]));
         }
         return left;
     }
     else if (cmd.find(';') != std::string::npos)
     {
         auto parts = split_once(cmd, ";");
         eval_command(trim(parts[0]));
         return eval_command(trim(parts[1]));
     }
     else if (cmd.find(">>") != std::string::npos)
     {
         auto parts = split_once(cmd, ">>");
         std::string left = trim(parts[0]);
         std::string file = trim(parts[1]);
         int fd = open(file.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
         if (fd == -1)
         {
             std::cerr << "Failed to open file for append: " << file << " : " << std::strerror(errno) << std::endl;
             return 1;
         }
         int rc = run_command(left, fd);
         close(fd);
         return rc;
     }
     else if (cmd.find('>') != std::string::npos)
     {
         auto parts = split_once(cmd, ">");
         std::string left = trim(parts[0]);
         std::string file = trim(parts[1]);
         int fd = open(file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
         if (fd == -1)
         {
             std::cerr << "Failed to open file for write: " << file << " : " << std::strerror(errno) << std::endl;
             return 1;
         }
         int rc = run_command(left, fd);
         close(fd);
         return rc;
     }
     else
     {
         bool is_silent = false;
         if (cmd.size() >= 6 && cmd.compare(0, 6, "silent") == 0)
         {
             if (cmd.size() == 6)
             {
                 return 0;
             }
             if (cmd[6] == ' ' || cmd[6] == '\t')
             {
                 is_silent = true;
                 cmd = trim(cmd.substr(6));
             }
         }
         return run_command(cmd, 1, is_silent);
     }
 }

int main()
 {
     std::string cmd;
     for (;;)
     {
         std::cout << "[custom shell]-> " << std::flush;
         if (!std::getline(std::cin, cmd)) break;
         cmd = trim(cmd);
         if (cmd.empty()) continue;
         if (cmd == "exit") break;
         int rc = eval_command(cmd);
         static_cast<void>(rc);
     }
     return 0;
 }

