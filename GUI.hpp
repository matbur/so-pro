#pragma once

#include <vector>

#include "URL.hpp"

class GUI {
public:
    GUI(std::vector<URL> *urls, int max_threads);

    void operator()(std::mutex *mtx);

private:
    std::vector<URL> *_urls;
    int _max_threads;

    void _paint(size_t number, int len);
};

