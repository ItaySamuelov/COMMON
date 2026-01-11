#include <cerrno>
#include <cstring>
#include <iostream>
#include <sys/resource.h>  // setpriority
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static void burn_cpu(long long iters) {
    volatile long long x = 0;
    for (long long i = 0; i < iters; ++i) x += i;
}

static void child_work(int id, int nice_value) {
    // Try to change scheduling priority (nice). This may fail for negative values without permission.
    errno = 0;
    if (setpriority(PRIO_PROCESS, 0, nice_value) != 0) {
        std::cerr << "Child " << id << " setpriority(" << nice_value
                  << ") failed: " << std::strerror(errno) << "\n";
    }

    int actual_nice = getpriority(PRIO_PROCESS, 0);
    std::cout << "Child " << id << " started. pid=" << getpid()
              << " nice=" << actual_nice << "\n";

    // CPU-bound work (scheduler decides how CPU time is shared)
    burn_cpu(350000000LL);

    std::cout << "Child " << id << " finished. pid=" << getpid()
              << " nice=" << getpriority(PRIO_PROCESS, 0) << "\n";
    _exit(0);
}

int main() {
    std::cout << "Parent pid=" << getpid() << "\n";

    // Three children with different nice values.
    // Larger nice => lower priority. Smaller nice => higher priority.
    const int nice_vals[3] = {0, 10, 19};

    for (int i = 0; i < 3; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            std::cerr << "fork failed\n";
            return 1;
        }
        if (pid == 0) {
            child_work(i, nice_vals[i]);
        }
    }

    // Parent waits and reports the order children exit (this order is the "observable scheduling result").
    for (int k = 0; k < 3; ++k) {
        int status = 0;
        pid_t done = wait(&status);
        std::cout << "Parent: child exited pid=" << done
                  << " status=" << status << "\n";
    }

    std::cout << "Parent done.\n";
    return 0;
}
