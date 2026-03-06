#include "rw.h"
#include <iostream>
#include <thread>
#include <chrono>

ReaderWriters::ReaderWriters(): // trivial constructor
      sharedValue(0),
      activeReaders(0),
      waitingReaders(0),
      activeWriters(0),
      waitingWriters(0) {}

void ReaderWriters::startRead(int readerId) {
    // TODO:
    // Implement the reader entry protocol.
    //
    // Requirements:
    // 1. Multiple readers may read simultaneously.
    // 2. A reader must not enter while a writer is writing.
    // 3. Depending on the policy you choose, you may also block readers
    //    when writers are waiting.
    //
    // Suggested steps:
    // - lock the mutex
    // - update waitingReaders if needed
    // - wait on the condition variable until reading is allowed
    // - update activeReaders / waitingReaders
    // - print a message
    mtx.lock();
    waitingReaders++;
    while (activeWriters){
      cv.wait(&mtx)
    }
    waitingReaders--;
    activeReaders++;
    printMtx.lock()
    cout << "Reader " << readerId << " entered" << endl;
    printMtx.unlock()
    mtx.unlock();
}

void ReaderWriters::endRead(int readerId) {
    // TODO:
    // Implement the reader exit protocol.
    //
    // Suggested steps:
    // - lock the mutex
    // - decrement activeReaders
    // - print a message
    // - notify waiting threads if appropriate
    mtx.lock();
    activeReaders--;
    printMtx.lock();
    cout << "Reader " << readerId << " exited" << endl;
    printMtx.unlock();
    if (activeReaders == 0){
      cv.signal();// wake a single writer
    }
    mtx.unlock();
}

void ReaderWriters::startWrite(int writerId) {
    // TODO:
    // Implement the writer entry protocol.
    //
    // Requirements:
    // 1. Only one writer may write at a time.
    // 2. A writer must have exclusive access.
    //
    // Suggested steps:
    // - lock the mutex
    // - update waitingWriters if needed
    // - wait on the condition variable until writing is allowed
    // - update activeWriters / waitingWriters
    // - print a message
    mtx.lock();
    waitingWriters++;
    while(activeWriters || activeReaders){ // gets priority before waiting-readers!
      cv.wait(&mtx);
    }
    waitingWriters--;
    activeWriters++;
    printMtx.lock();
    cout << "Writer " << writerId << " entered" << endl;
    printMtx.unlock();
    mtx.unlock();
}

void ReaderWriters::endWrite(int writerId) {
    // TODO:
    // Implement the writer exit protocol.
    //
    // Suggested steps:
    // - lock the mutex
    // - decrement activeWriters
    // - print a message
    // - notify waiting threads

    (void)writerId;
}

int ReaderWriters::readValue(int readerId) {
    startRead(readerId);

    // Simulate reading
    std::this_thread::sleep_for(std::chrono::milliseconds(80));

    int value = sharedValue;

    {
        std::lock_guard<std::mutex> printLock(printMtx);
        std::cout << "[Reader " << readerId << "] read value = "
                  << value << std::endl;
    }

    endRead(readerId);
    return value;
}

void ReaderWriters::writeValue(int writerId, int newValue) {
    startWrite(writerId);

    // Simulate writing
    std::this_thread::sleep_for(std::chrono::milliseconds(120));

    sharedValue = newValue;

    {
        std::lock_guard<std::mutex> printLock(printMtx);
        std::cout << "[Writer " << writerId << "] wrote value = "
                  << newValue << std::endl;
    }

    endWrite(writerId);
}