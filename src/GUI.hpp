#pragma once

#include "common.hpp"
#include "URL.hpp"

class GUI {
public:
    GUI(std::vector<URL> *urls, int max_threads, std::mutex *mtx);

    void operator()();

private:
    std::vector<URL> *_urls;
    int _max_threads;
    size_t _number;
    int _max_len;
    int _height;

    void _paint();
};

