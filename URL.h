//
// Created by matbur on 3/31/17.
//

#include <curl/curl.h>
#include <fstream>
#include <iostream>
#include <mutex>
#include <ncurses.h>

#include "Semaphore.h"

#pragma once

class URL {
public:
    int _id;
    std::string _url;
    int _len;
    std::string _path;
    double _progress;
    int _pipes;
    bool _done;
    std::mutex *_mtx;

public:
    URL(int id, std::string url, std::string path, std::mutex *mtx);

    void operator()(Semaphore *s);
};

size_t data_write(void *ptr, size_t size, size_t nmemb, void *userdata);

void progress_callback(URL *clientp, double dltotal, double dlnow);

