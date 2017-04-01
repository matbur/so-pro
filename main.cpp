#include <ncurses.h>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <curl/curl.h>
#include <iostream>
#include <thread>
#include <mutex>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

std::mutex gui_mutex;
std::mutex urls_mutex;


class URL_Thread {
public:
    int _id;
    std::string _url;
    int _len;
    std::string _path;
    double _progress;
    int _pipes;
    bool _done;

public:
    URL_Thread(int id, std::string url, std::string path);

    void operator()();
};

size_t data_write(void *ptr, size_t size, size_t nmemb, void *userdata);

void progress_callback(URL_Thread *clientp, double dltotal, double dlnow, double ultotal, double ulnow);

URL_Thread::URL_Thread(int id, std::string url, std::string path)
        : _id(id), _url(url), _path(path), _progress(0.), _pipes(0) {
    _len = (int) url.size();
    _done = false;
//    std::cout << path << ' ' << this << '\n';
}

void URL_Thread::operator()() {
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
    std::lock_guard<std::mutex> lk(urls_mutex);

    clientp->_progress = progress;
    clientp->_pipes = (int) progress / 10;
    if (dltotal == dlnow && dltotal > 0) {
        clientp->_done = true;
    }
//    printf("%s %.2f\n", clientp->_path.c_str(), progress);
//    std::cout << clientp->_path << ' ' << clientp << '\n';
}

std::vector<URL_Thread> urls;

int fun_with_ncurses() {

    initscr();
    raw();
    printw("Hello world");
    addch('a');

    move(10, 10);
    attron(A_STANDOUT | A_UNDERLINE);
    printw("Hello %d", 5);
    attroff(A_STANDOUT | A_UNDERLINE);

    mvprintw(20, 20, "d");
    mvaddch(21, 21, 'x');

    printw("Hello world");

    getch();
    endwin();

    return 0;
}

struct {
    std::string input_file;
    int max_threads;
    std::string output_dir;
} arg;

int parse_args(int argc, const char *const *argv) {
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help,h", "show this message and exit")
            ("input_file,i", po::value<std::string>(&arg.input_file), "(REQUIRED) file with urls")
            ("thread_num,n", po::value<int>(&arg.max_threads), "(REQUIRED) number of threads")
            ("output_dir,o", po::value<std::string>(&arg.output_dir)->default_value("output"), "destination directory");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help") || !vm.count("input_file") || !vm.count("thread_num")) {
        std::cout << desc;
        return 1;
    }

    return 0;
}

int manage_files() {
    if (!fs::is_regular_file(arg.input_file)) {
        printf("No such file: %s\n", arg.input_file.c_str());
        return 1;
    }

    if (!fs::is_directory(arg.output_dir)) {
        try {
            return !fs::create_directories(arg.output_dir);
        } catch (fs::filesystem_error) {
            printf("Couldn't create directory: %s\n", arg.output_dir.c_str());
            return 0;
        }
    }

    return 0;
}

int read_urls() {
    std::ifstream file(arg.input_file);
    std::string url, fname;

    urls_mutex.lock();
    auto url_id = 0;
    while (file >> url >> fname) {
        if (url[0] == '#')
            continue;

        auto path = arg.output_dir + "/" + fname;
        urls.push_back(URL_Thread{url_id++, url, path});
    }
    urls_mutex.unlock();

    return 0;
}

void paint() {
    erase();

//    std::lock_guard<std::mutex> lk(urls_mutex);
    std::lock_guard<std::mutex> lk(gui_mutex);
    for (auto url: urls) {
        move(url._id, 0);
        printw(url._path.c_str());
        addch(' ');
        printw("%d %.2f", url._pipes, url._progress);
//        for (auto i = 0; i < url._pipes; i++) {
//            addch('|');
//        }
    }

    refresh();
}

void gui_func() {
    initscr();
    raw();

    bool done = false;
    while (!done) {
//        gui_mutex.lock();
//        urls_mutex.lock();
        done = true;
        for (auto url: urls) {
            if (!url._done) {
                done = false;
                break;
            }
        }
//        urls_mutex.unlock();
        paint();
//        gui_mutex.unlock();
    }

    getch();
    endwin();
}

void print_urls() {
    urls_mutex.lock();
    auto n = urls.size();
    urls_mutex.unlock();

    auto plural_f = n == 1 ? "" : "s";
    auto plural_t = arg.max_threads == 1 ? "" : "s";
    printw("Downloading %d file%s by %d thread%s",
           n, plural_f, arg.max_threads, plural_t);

    auto len = 0;
    for (auto url: urls) {
        len = std::max(url._len, len);
    }

    for (auto url: urls) {
        const auto y = url._id + 1;
        move(y, 0);
        printw(url._url.c_str());
        move(y, len + 1);
        addch('[');
        move(y, len + 21);
        printw("]  0.00 %%");
    }
}


class MyThread {
    int _x;
    int _pro;
public:
    MyThread(int x) : _x(x) {}

    void operator()(int &n) {
        for (auto i = 0; i < 100; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
//            foo();
            gui_mutex.lock();
            n++;
            if (n == 90) {
//                num--;
            }
            gui_mutex.unlock();

        }
    }
};


int main(int argc, const char *const *argv) {
    if (auto ret = parse_args(argc, argv)) {
        return ret;
    }

    if (auto ret = manage_files()) {
        return ret;
    }

    read_urls();
//    for (auto url:urls) {
//        std::cout << url._path << ' ' << &url << '\n';
//    }
//    print_urls();

//    printf("args: -i |%s| -n |%d| -o |%s|\n", arg.input_file.c_str(), arg.max_threads, arg.output_dir.c_str());
//    std::cout << "|" << arg.input_file << "|" << arg.max_threads << "|" << arg.output_dir << "|\n";

//    URL_Thread(1, "http://cachefly.cachefly.net/10mb.test", "/home/matbur/tmp/bigfile")();
    std::thread gui_thread(gui_func);

    std::vector<std::thread> threads;
    for (auto &url: urls) {
        threads.push_back(std::thread(std::ref(url)));
    }

    for (auto &t: threads) {
        t.join();
    }

    gui_thread.join();

    return 0;
}