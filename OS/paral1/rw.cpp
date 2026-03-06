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
    std::unique_lock<std::mutex> lock(mtx);
    waitingReaders++;
    while (activeWriters || waitingWriters){ // waiting writers get priority over waiting readers
        cvr.wait(lock);
    }
    waitingReaders--;
    activeReaders++;
    printMtx.lock();
    std::cout << "Reader " << readerId << " entered" << std::endl;
    printMtx.unlock();
    lock.unlock();
}

void ReaderWriters::endRead(int readerId) {
    std::unique_lock<std::mutex> lock(mtx);
    activeReaders--;
    printMtx.lock();
    std::cout << "Reader " << readerId << " exited" << std::endl;
    printMtx.unlock();

    if (!activeReaders){
        if (waitingWriters){
            cvw.notify_one(); // wake a ONE writer
        }
        else{
            cvr.notify_all(); // wake up ALL readers
        }
    }
    lock.unlock();
}

void ReaderWriters::startWrite(int writerId) {
    std::unique_lock<std::mutex> lock(mtx);
    waitingWriters++;
    while(activeWriters || activeReaders){ // gets priority before waiting-readers!
        cvw.wait(lock);
    }
    mtx.lock();
    waitingWriters--;
    activeWriters++;
    printMtx.lock();
    std::cout << "Writer " << writerId << " entered" << std::endl;
    printMtx.unlock();
    lock.unlock();
}

void ReaderWriters::endWrite(int writerId) {
    std::unique_lock<std::mutex> lock(mtx);
    activeWriters--;
    printMtx.lock();
    std::cout << "Writer " << writerId << " exited" << std::endl;
    printMtx.unlock();

    if (activeWriters){exit(1);} /// should never happen.
    if (waitingWriters){ //waiting writers get priority over waiting readers
        cvw.notify_one(); //wake up ONE waiting writer
    }
    else{
        cvr.notify_all(); // wake up ALL waiting readers
    }
    lock.unlock();
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

    printMtx.lock();
    std::cout << "[Writer " << writerId << "] wrote value = " << newValue << std::endl;
    printMtx.unlock();

    endWrite(writerId);
}