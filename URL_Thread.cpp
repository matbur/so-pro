//
// Created by matbur on 3/31/17.
//

#include "URL_Thread.h"


int URL_Thread::get_id() const {
    return _id;
}

const std::string &URL_Thread::get_url() const {
    return _url;
}

float URL_Thread::get_size() const {
    return _size;
}

void URL_Thread::set_size(float _size) {
    URL_Thread::_size = _size;
}

float URL_Thread::get_downloaded() const {
    return _downloaded;
}

void URL_Thread::set_downloaded(float _downloaded) {
    URL_Thread::_downloaded = _downloaded;
}

bool URL_Thread::is_is_done() const {
    return _is_done;
}

void URL_Thread::set_is_done(bool _is_done) {
    URL_Thread::_is_done = _is_done;
}

int URL_Thread::get_thread_id() const {
    return _thread_id;
}

void URL_Thread::set_thread_id(int _thread_id) {
    URL_Thread::_thread_id = _thread_id;
}

const std::string &URL_Thread::get_path() const {
    return _path;
}

void URL_Thread::set_path(const std::string &_path) {
    URL_Thread::_path = _path;
}
