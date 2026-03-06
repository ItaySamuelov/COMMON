#ifndef RW_H
#define RW_H

#include <mutex>
#include <condition_variable>

class ReaderWriters {
public:
    ReaderWriters();

    // Called before a reader enters the critical section
    void startRead(int readerId);

    // Called after a reader exits the critical section
    void endRead(int readerId);

    // Called before a writer enters the critical section
    void startWrite(int writerId);

    // Called after a writer exits the critical section
    void endWrite(int writerId);

    // High-level operations that use the synchronization protocol
    int readValue(int readerId);
    void writeValue(int writerId, int newValue);

private:
    // Shared resource
    int sharedValue;

    // Synchronization state
    // The exact meaning and use of these fields is part of the exercise.
    int activeReaders;
    int waitingReaders;
    int activeWriters;
    int waitingWriters;

    std::mutex mtx;
    std::condition_variable cv;

    // Used only to avoid interleaved prints
    std::mutex printMtx;
};

#endif