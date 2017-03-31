//
// Created by matbur on 3/31/17.
//

#include <iostream>

#pragma once


class URL_Thread {
    int _id;
    std::string _url;
    float _size;
    float _downloaded;
    bool _is_done;
    int _thread_id;
    std::string _path;

public:
    URL_Thread(int id, std::string url) : _id(id), _url(url) {}

    int get_id() const;

    const std::string &get_url() const;

    float get_size() const;

    void set_size(float _size);

    float get_downloaded() const;

    void set_downloaded(float _downloaded);

    bool is_is_done() const;

    void set_is_done(bool _is_done);

    int get_thread_id() const;

    void set_thread_id(int _thread_id);

    const std::string &get_path() const;

    void set_path(const std::string &_path);

private:
    std::string _error;

};

