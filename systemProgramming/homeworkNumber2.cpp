#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <cstdlib>
#include <cstring>

#define FILENAME_LENGTH 50

void compute_collatz_sequence(int start, int end, int factor, int child_pid, int parent_pid)
{
    char filename[FILENAME_LENGTH];
    snprintf(filename, FILENAME_LENGTH, "results_%d_%d.dat", child_pid, parent_pid);
    std::ofstream fp(filename);
    if (!fp.is_open())
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    for (int j = start; j < end; j++)
    {
        int n = j;
        while (n != 1)
        {
            fp << n << " ";
            if (n % 2 == 0)
            {
                n /= 2;
            }
            else
            {
                n = 3 * n + 1;
            }
        }
        fp << "1\n";
    }
    fp.close();
    std::cout << "I am child number " << child_pid << ", my parent is " << parent_pid << ", I computed the collatz sequence for numbers from " << start << " to " << end - 1 << std::endl;
}

void output_collatz_sequence(int min_num, int max_num, const std::vector<pid_t> &child_pids, int parent_pid)
{
    for (int j = 0; j < child_pids.size(); j++)
    {
        char filename[FILENAME_LENGTH];
        snprintf(filename, FILENAME_LENGTH, "results_%d_%d.dat", child_pids[j], parent_pid);
        std::ifstream fp(filename);
        if (!fp.is_open())
        {
            std::cerr << "Error opening file " << filename << ": " << strerror(errno) << std::endl;
            continue;
        }
        std::string line;
        while (std::getline(fp, line))
        {
            std::istringstream iss(line);
            int result;
            while (iss >> result)
            {
                std::cout << result << " "; // output each number read from the file
            }

            std::cout << std::endl;
        }
        fp.close();
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <num_children> <min_num> <max_num>" << std::endl;
        return 1;
    }
    int num_children = atoi(argv[1]); // number of child processes to create
    if (num_children <= 0 || num_children > 10)
    {
        std::cerr << "Error: <num_children> must be a positive integer less than or equal to 10" << std::endl;
        return 1;
    }
    int min_num = atoi(argv[2]); // minimum number to compute Collatz sequence
    int max_num = atoi(argv[3]); // maximum number to compute Collatz sequence

    if (min_num >= max_num)
    {
        std::cerr << "Error: <min_num> must be less than <max_num>" << std::endl;
        return 1;
    }

    int parent_pid = getpid();

    int first_half = num_children / 2;
    int second_half = num_children - first_half;
    int x = (max_num - min_num + 1) / 3;
    int first_half_size = x / first_half;
    int second_half_size = (2 * x) / second_half;
    int remainder = (max_num - min_num + 1) % first_half;
    int start = min_num;

    std::vector<pid_t> child_pids(num_children);

    for (int i = 0; i < first_half; i++)
    {
        pid_t pid = fork(); // create a new child process
        if (pid == -1)
        { // fork error
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        { // child process
            // Calculate the range of numbers to compute Collatz sequences for
            int end = start + first_half_size;
            if (i == first_half - 1)
            {
                end += remainder;
            }
            // Compute Collatz sequences for
            compute_collatz_sequence(start, end, 1, getpid(), getppid());
            exit(EXIT_SUCCESS); // exit child process
        }
        child_pids[i] = pid;
        start += first_half_size + (i < remainder ? 1 : 0);
    }

    start = min_num + x;

    for (int i = 0; i < second_half; i++)
    {
        pid_t pid = fork(); // create a new child process
        if (pid == -1)
        { // fork error
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        { // child process

            int end = start + second_half_size;
            if (i == second_half - 1)
            {
                end = max_num; // last child process handles the remainder of the range
            }
            // Compute Collatz sequences for the range of numbers
            compute_collatz_sequence(start, end, 2, getpid(), getppid());
            exit(EXIT_SUCCESS); // exit child process
        }
        child_pids[first_half + i] = pid;
        start += second_half_size;
    }

    int num_completed = 0;
    while (num_completed < num_children)
    {
        pid_t pid;
        int status;
        if ((pid = waitpid(-1, &status, 0)) == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }
        }
        num_completed++;

        if (WIFEXITED(status))
        {
            std::cout << "Child process " << pid << " exited with status " << WEXITSTATUS(status) << std::endl;
        }
        else if (WIFSIGNALED(status))
        {
            std::cout << "Child process " << pid << " terminated due to signal " << WTERMSIG(status) << std::endl;
        }
        else
        {
            std::cout << "Child process " << pid << " did not terminate normally" << std::endl;
        }
    }
    // Read results from children files and output Collatz sequence for each number
    std::cout << "Parent will print output here: " << std::endl;

    output_collatz_sequence(min_num, max_num, child_pids, parent_pid);

    return 0;
}
