//
// Created by matbur on 3/31/17.
//

#include "URL_Thread.h"


URL_Thread::URL_Thread(int id, std::string url, std::string path, std::mutex *mtx)
        : _id(id), _url(url), _path(path), _mtx(mtx) {
    _len = (int) url.size();
    _progress = 0.;
    _pipes = 0;
    _done = false;
}

void URL_Thread::operator()(Semaphore *s) {
    s->wait();

    auto curl = curl_easy_init();
    if (curl == nullptr)
        return;

    std::ofstream outfile(_path);

    curl_easy_setopt(curl, CURLOPT_URL, _url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outfile);
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_callback);
    curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

    curl_easy_perform(curl);

    outfile.close();
    curl_easy_cleanup(curl);

    s->notify();
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

void progress_callback(URL_Thread *clientp, double dltotal, double dlnow, double ultotal, double ulnow) {
    auto progress = 100 * dlnow / dltotal;

    std::lock_guard<std::mutex> lk(*(clientp->_mtx));

    clientp->_progress = progress;
    clientp->_pipes = (int) progress / 10;
    if (dltotal == dlnow && dltotal > 0) {
        clientp->_done = true;
    }
}

