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
    mtx.lock();
    waitingReaders++;
    while (activeWriters || waitingWriters){ // waiting writers get priority over waiting readers
      cv.wait(&mtx)
    }
    waitingReaders--;
    activeReaders++;
    printMtx.lock();
    cout << "Reader " << readerId << " entered" << endl;
    printMtx.unlock();
    mtx.unlock();
}

void ReaderWriters::endRead(int readerId) {
    mtx.lock();
    activeReaders--;
    printMtx.lock();
    cout << "Reader " << readerId << " exited" << endl;
    printMtx.unlock();
    if (activeReaders == 0){
      cv.notify_one()// wake a single writer
    }
    mtx.unlock();
}

void ReaderWriters::startWrite(int writerId) {
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
    mtx.lock();
    activeWriters--;
    printMtx.lock();
    cout << "Writer " << writerId << " exited" << endl;
    printMtx.unlock();
    if (activeWriters){exit(1)} /// should never happen.
    if (waitingWriters){ //waiting writers get priority over waiting readers
        cv.notify_one() //wake up ONE waiting writer
    }
    else{
        cv.notify_all(); // wake up ALL waiting readers
    }
    printMtx.unlock();
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