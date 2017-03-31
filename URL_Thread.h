//
// Created by matbur on 3/31/17.
//

#include <iostream>
#include <curl/curl.h>

#pragma once


class URL_Thread {
    int _id;
    std::string _url;
    int _len;
    float _size;
    float _downloaded_in;
    bool _is_done;
    int _thread_id;
    std::string _path;
    std::string _error;

public:
    URL_Thread(int id, std::string url);

    int get_id() const;

    const std::string &get_url() const;

    int get_len() const;

    float get_size() const;

    void set_size(float _size);

    float get_downloaded_in() const;

    void set_downloaded_in(float _downloaded);

    bool is_is_done() const;

    void set_is_done(bool _is_done);

    int get_thread_id() const;

    void set_thread_id(int _thread_id);

    const std::string &get_path() const;

    void set_path(const std::string &_path);

    void operator()();
};

size_t data_write(void *ptr, size_t size, size_t nmemb, void *userdata);

