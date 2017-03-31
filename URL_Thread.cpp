//
// Created by matbur on 3/31/17.
//

#include <fstream>
#include "URL_Thread.h"

URL_Thread::URL_Thread(int id, std::string url)
        : _id(id), _url(url) {
    _len = (int) url.size();
    _path = "/home/matbur/tmp/bigfile";
}

int URL_Thread::get_id() const {
    return _id;
}

const std::string &URL_Thread::get_url() const {
    return _url;
}

int URL_Thread::get_len() const {
    return _len;
}

float URL_Thread::get_size() const {
    return _size;
}

void URL_Thread::set_size(float _size) {
    URL_Thread::_size = _size;
}

float URL_Thread::get_downloaded_in() const {
    return _downloaded_in;
}

void URL_Thread::set_downloaded_in(float _downloaded) {
    URL_Thread::_downloaded_in = _downloaded;
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

void URL_Thread::operator()() {
    auto curl = curl_easy_init();

    if (curl == nullptr)
        return;

    std::ofstream outfile(_path);

    curl_easy_setopt(curl, CURLOPT_URL, _url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_write);
    curl_easy_setopt(curl, CURLOPT_FILE, &outfile);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

    curl_easy_perform(curl);

    outfile.close();
    curl_easy_cleanup(curl);
}


size_t data_write(void *ptr, size_t size, size_t nmemb, void *userdata) {
    if (userdata) {
        auto &os = *(std::ostream *) userdata;
        auto len = size * nmemb;
        if (os.write((char *) ptr, len))
            return len;
    }
    return 0;
}
