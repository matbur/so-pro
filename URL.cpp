#include "URL.hpp"

URL::URL(int id, std::string url, std::string path, std::mutex *mtx)
        : _id(id), _url(url), _path(path), _mtx(mtx) {
    _len = (int) path.size();
    _progress = 0;
    _total = 0.;
    _now = 0.;
    _done = false;
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

int URL::get_progress() const {
    return _progress;
}

double URL::get_total() const {
    return _total;
}

double URL::get_now() const {
    return _now;
}

bool URL::is_done() const {
    return _done;
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

    auto progress = int(100 * dlnow / dltotal);

    if (dltotal == 0)
        progress = 0;

    std::lock_guard<std::mutex> lg(*(clientp->_mtx));

    clientp->_progress = progress;
    clientp->_total = dltotal;
    clientp->_now = dlnow;

    if (dltotal == dlnow && dltotal > 0) {
        clientp->_done = true;
    }
}

