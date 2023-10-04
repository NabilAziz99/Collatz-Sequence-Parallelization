# Collatz-Sequence-Parallelization

The Collatz-Sequence-Parallelization project is a demonstration of utilizing parallel processing for calculating the Collatz sequence over a given range of numbers. By employing multiple child processes, each responsible for a subset of the range, the program achieves efficient parallel computation.

## 🌟 Features

- **Parallel Computation**: Efficiently computes the Collatz sequence over a range of numbers using multiple child processes.
- **File I/O**: Each child process writes its results to a uniquely identifiable file. The parent process collates these results for a comprehensive sequence.
- **Error Handling**: Robust error handling mechanisms for both file I/O and the `fork()` system call.

## 🚀 Usage

To utilize the program:

```bash
./collatz <num_children> <min_num> <max_num> 
num_children: Number of child processes to be spawned (must be a positive integer ≤ 10).
min_num: The starting number for which the Collatz sequence will be computed.
max_num: The ending number for which the Collatz sequence will be computed.
Example:
To compute the Collatz sequence for numbers ranging from 1 to 100 using 4 child processes:
./collatz 4 1 100

🔍 Implementation Details
Process Creation: The program employs the fork() system call to generate multiple child processes.

Range Allocation: The parent process determines and allocates a specific range of numbers to each child process based on the given inputs.

File Storage: Each child process writes its results to a file uniquely named with its process ID and the parent process ID.

Results Compilation: Once all child processes are done, the parent process reads each file and consolidates the results, presenting the Collatz sequence for the entire range.
