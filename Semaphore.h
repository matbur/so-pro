//
// Created by matbur on 4/1/17.
//

# pragma once

#include <mutex>
#include <condition_variable>

class Semaphore {
    std::mutex _mtx;
    std::condition_variable _cv;
    int _working_threads;
    int _max_threads;

public:
    Semaphore(int count);

    void notify();

    void wait();
};
