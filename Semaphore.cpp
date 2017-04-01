//
// Created by matbur on 4/1/17.
//

#include "Semaphore.h"

Semaphore::Semaphore(int count) : _max_threads(count) {
    _working_threads = 0;
}

void Semaphore::notify() {
    std::unique_lock<std::mutex> lock(_mtx);
    _working_threads--;
    _cv.notify_one();
}

void Semaphore::wait() {
    std::unique_lock<std::mutex> lock(_mtx);
    _cv.wait(lock, [&] { return _working_threads < _max_threads; });
    _working_threads++;
}
