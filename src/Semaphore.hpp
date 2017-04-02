# pragma once

#include "common.hpp"

class Semaphore {
public:
    Semaphore(int count);

    void notify();

    void wait();

private:
    std::mutex _mtx;
    std::condition_variable _cv;
    int _working_threads;
    int _max_threads;
};
