#pragma once

#include <curl/curl.h>
#include <fstream>
#include <iostream>
#include <mutex>
#include <ncurses.h>

#include "Semaphore.hpp"

class URL {
public:
    URL(int id, std::string url, std::string path, std::mutex *mtx);

    void operator()(Semaphore *s);

    int get_id() const;

    int get_len() const;

    const std::string &get_path() const;

    double get_progress() const;

    int get_pipes() const;

    bool is_done() const;

private:
    int _id;
    std::string _url;
    int _len;
    std::string _path;
    double _progress;
    int _pipes;
    bool _done;
    std::mutex *_mtx;

    static size_t _data_write(void *ptr, size_t size, size_t nmemb, void *userdata);

    static void _progress_callback(URL *clientp, double dltotal, double dlnow);
};


