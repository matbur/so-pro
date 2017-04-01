#include "URL.hpp"

URL::URL(int id, std::string url, std::string path, std::mutex *mtx)
        : _id(id), _url(url), _path(path), _mtx(mtx) {
    _len = (int) path.size();
    _progress = 0.;
    _pipes = 0;
    _done = false;
}

void URL::operator()(Semaphore *s) {
    s->wait();

    auto curl = curl_easy_init();
    if (curl == nullptr)
        return;

    std::ofstream outfile(_path);

    curl_easy_setopt(curl, CURLOPT_URL, _url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _data_write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outfile);
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, _progress_callback);
    curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

    curl_easy_perform(curl);

    outfile.close();
    curl_easy_cleanup(curl);

    s->notify();
}

size_t URL::_data_write(void *ptr, size_t size, size_t nmemb, void *userdata) {
    if (userdata) {
        auto &os = *(std::ostream *) userdata;
        auto len = size * nmemb;
        if (os.write((char *) ptr, len))
            return len;
    }
    return 0;
}

void URL::_progress_callback(URL *clientp, double dltotal, double dlnow) {
    auto progress = 100 * dlnow / dltotal;

    std::lock_guard<std::mutex> lk(*(clientp->_mtx));

    clientp->_progress = progress;
    clientp->_pipes = (int) progress / 10;
    if (dltotal == dlnow && dltotal > 0) {
        clientp->_done = true;
    }
}

int URL::get_id() const {
    return _id;
}

int URL::get_len() const {
    return _len;
}

const std::string &URL::get_path() const {
    return _path;
}

double URL::get_progress() const {
    return _progress;
}

int URL::get_pipes() const {
    return _pipes;
}

bool URL::is_done() const {
    return _done;
}

