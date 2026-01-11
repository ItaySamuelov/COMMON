// ex2.cpp
// Build: make
// Run examples:
//   ./ex2 3 50          # 3 children, 50ms quantum
//   ./ex2 5 20

#include <chrono>
#include <csignal>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

using namespace std;

static void burn_cpu_chunk(int child_id) {
    // Some CPU work. Keep it deterministic-ish.
    volatile unsigned long long x = 0;
    for (unsigned long long i = 0; i < 30'000'000ULL; ++i) {
        x += (i ^ (child_id + 1));
    }
    (void)x;
}

static void child_main(int child_id) {
    // Child prints occasionally so you can see scheduling.
    // (Note: printing is slow, so don't print too much.)

    cout << "Child " << child_id << " ready. pid=" << getpid() << "\n";

    // Optional: start paused until parent begins scheduling.
    // Parent will SIGCONT us, but we can also self-stop once at start.
    raise(SIGSTOP);

    // TODO (YOU): Decide how a child knows when to exit.
    // Options:
    // 1) Run for a fixed number of chunks (e.g., 50 chunks).
    // 2) Run until parent sends SIGTERM (then handle it).
    // 3) Run until a time limit (measure with clock).

    const int CHUNKS_TO_RUN = 25; // You can change this.
    for (int k = 0; k < CHUNKS_TO_RUN; ++k) {
        burn_cpu_chunk(child_id);

        // Print progress sometimes (not every chunk).
        if (k % 5 == 0) {
            cout << "Child " << child_id << " progress chunk=" << k
                 << " pid=" << getpid() << "\n";
        }
    }

    cout << "Child " << child_id << " done. pid=" << getpid() << "\n";
    _exit(0);
}

static void ms_sleep(int ms) {
    this_thread::sleep_for(chrono::milliseconds(ms));
}

int main(int argc, char** argv) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <num_children> <quantum_ms>\n";
        return 1;
    }

    int n = stoi(argv[1]);
    int quantum_ms = stoi(argv[2]);

    if (n <= 0 || n > 50 || quantum_ms <= 0 || quantum_ms > 2000) {
        cerr << "Bad args.\n";
        return 1;
    }

    vector<pid_t> pids;
    pids.reserve(n);

    cout << "Parent pid=" << getpid() << " starting " << n
         << " children, quantum=" << quantum_ms << "ms\n";

    // Fork children
    for (int i = 0; i < n; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            cerr << "fork failed\n";
            return 1;
        }
        if (pid == 0) {
            child_main(i);
        }
        pids.push_back(pid);
    }

    // Ensure all children are stopped before we begin scheduling.
    // They self-stop with raise(SIGSTOP). But in general you'd confirm.
    // For simplicity, we wait a tiny bit.
    ms_sleep(50);

    // Parent: round-robin scheduling loop.
    // We rotate through children; each gets quantum_ms to run.
    // When a child exits, remove it from the rotation.

    vector<bool> alive(n, true);
    int alive_count = n;
    int idx = 0;

    while (alive_count > 0) {
        // Skip dead ones
        if (!alive[idx]) {
            idx = (idx + 1) % n;
            continue;
        }

        pid_t pid = pids[idx];

        // Resume this child
        kill(pid, SIGCONT);

        // Let it run for a quantum
        ms_sleep(quantum_ms);

        // Pause it again (if still alive)
        // NOTE: If it already exited, kill() will fail (ESRCH). That's ok.
        kill(pid, SIGSTOP);

        // Reap any exited children (non-blocking)
        // TODO (YOU): Improve this logic:
        // - mark specific child as dead when it exits
        // - print exit order
        // - handle multiple exits per iteration properly
        while (true) {
            int status = 0;
            pid_t done = waitpid(-1, &status, WNOHANG);
            if (done == 0) break;      // no more exited children
            if (done < 0) break;       // error or no children

            // Find which child this pid belonged to
            for (int j = 0; j < n; ++j) {
                if (alive[j] && pids[j] == done) {
                    alive[j] = false;
                    alive_count--;
                    cout << "Parent: child idx=" << j << " pid=" << done
                         << " exited. status=" << status << "\n";
                    break;
                }
            }
        }

        idx = (idx + 1) % n;
    }

    cout << "Parent: all children finished.\n";
    return 0;
}
