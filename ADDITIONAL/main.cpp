#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

#define BUF_SIZE 65536 // 64 KB buffer per file

struct Item
{
    string line;
    int idx;
};

struct Cmp
{
    bool operator()(const Item &a, const Item &b) const
    {
        return a.line > b.line; // reverse priority
    }
};

// Read the next line from fd into line
bool read_line(int fd, string& leftover, string& line)
{
    char buf[BUF_SIZE];
    while (true)
    {
        size_t pos = leftover.find('\n');
        if (pos != string::npos)
        {
            line = leftover.substr(0, pos);
            leftover = leftover.substr(pos + 1);
	    if(line.empty())
	    {
		    continue;
	    }
            return true;
        }

        ssize_t n = read(fd, buf, BUF_SIZE);
        if (n < 0)
        {
            perror("read");
            return false;
        }
        else if (n == 0)
        {
            if (!leftover.empty())
            {
                line = leftover;
                leftover.clear();
                return true;
            }
            return false;
        }
        leftover += string(buf, n);
    }
}

// Write a line with newline to fd
bool write_line(int fd, const string& line)
{
    string tmp = line + '\n';
    ssize_t n = write(fd, tmp.c_str(), tmp.size());
    return n == (ssize_t)tmp.size();
}

// Merge sorted runs into output file
bool merge_runs(const vector<string>& runs, const string& out_path)
{
    vector<int> fds;
    vector<string> leftovers;

    for (const auto& f : runs)
    {
        int fd = open(f.c_str(), O_RDONLY);
        if (fd < 0)
        {
            perror(f.c_str());
            return false;
        }
        fds.push_back(fd);
	leftovers.push_back("");
    }

    int out_fd = open(out_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd < 0)
    {
        perror("output file");
        return false;
    }

    priority_queue<Item, vector<Item>, Cmp> pq;

    // initialize heap with first line from each run
    for (int i = 0; i < (int)fds.size(); ++i)
    {
        string line;
        if (read_line(fds[i], leftovers[i], line))
        {
            pq.push(Item{line, i});
        }
    }

    // merge loop
    while (!pq.empty())
    {
        Item top = pq.top();
        pq.pop();

        if (!write_line(out_fd, top.line))
        {
            cerr << "write failed\n";
            return false;
        }

        string next;
        if (read_line(fds[top.idx], leftovers[top.idx], next))
        {
            pq.push(Item{next, top.idx});
        }
    }

    // close all files
    for (int fd : fds) close(fd);
    close(out_fd);

    // delete temporary files
    for (const auto& f : runs) unlink(f.c_str());

    cout << "Merging completed. Output: " << out_path << endl;
    return true;
}

int main(int argc, char * argv[])
{
    if (argc < 5)
    {
        cerr << "usage: " << argv[0] << " input output R_in_MB tempdir\n";
        return 1;
    }

    string in_path = argv[1];
    string out_path = argv[2];
    long long R_mb = stoll(argv[3]);
    string tmp_dir = argv[4];

    long long R = R_mb * 1024LL * 1024LL;
    long long chunk_size = (long long)(R * 0.7); // 70%

    int in_fd = open(in_path.c_str(), O_RDONLY);
    if (in_fd < 0)
    {
        perror("input file");
        return 2;
    }

    vector<string> buffer;
    vector<string> runs;
    buffer.reserve(1024);
    runs.reserve(1024);

    string leftover;
    string line;
    size_t run_id = 0;
    long long cur_bytes = 0;

    // read input file line by line
    while (read_line(in_fd, leftover, line))
    {
        buffer.push_back(line);
        cur_bytes += line.size() + 1;

        if (cur_bytes >= chunk_size)
        {
            sort(buffer.begin(), buffer.end());

            string tmpname = tmp_dir + "/run_" + to_string(run_id) + ".txt";
            int tmp_fd = open(tmpname.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (tmp_fd < 0)
            {
                perror(tmpname.c_str());
                return 3;
            }

            for (auto& s : buffer) write_line(tmp_fd, s);
            close(tmp_fd);

            runs.push_back(tmpname);
            cout << "Created run " << run_id << " -> " << tmpname << endl;

            run_id++;
            buffer.clear();
            cur_bytes = 0;
        }
    }

    // if something is still in buffer
    if (!buffer.empty())
    {
        sort(buffer.begin(), buffer.end());

        string tmpname = tmp_dir + "/run_" + to_string(run_id) + ".txt";
        int tmp_fd = open(tmpname.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        for (auto & s : buffer) write_line(tmp_fd, s);
        close(tmp_fd);

        runs.push_back(tmpname);
        cout << "Created run " << run_id << " -> " << tmpname << endl;

        run_id++;
        buffer.clear();
    }

    close(in_fd);

    // merge all runs
    if (!merge_runs(runs, out_path))
    {
        cerr << "Merging failed!\n";
        return 4;
    }

    return 0;
}

