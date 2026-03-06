#include "rw.h"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

void readerTask(ReaderWriters& rw, int id, int rounds) {
    for (int i = 0; i < rounds; ++i) {
        rw.readValue(id);
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
}

void writerTask(ReaderWriters& rw, int id, int rounds) {
    printf("hi"); // DEBUG
    for (int i = 0; i < rounds; ++i) {
        rw.writeValue(id, id * 100 + i);
        std::this_thread::sleep_for(std::chrono::milliseconds(90));
    }
}

int main() {
    ReaderWriters rw;
    std::vector<std::thread> threads;

    // Readers
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(readerTask, std::ref(rw), i + 1, 4);
    }

    // Writers
    for (int i = 0; i < 2; ++i) {
        threads.emplace_back(writerTask, std::ref(rw), i + 1, 4);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "All threads finished." << std::endl;
    return 0;
}