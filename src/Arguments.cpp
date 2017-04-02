#include "Arguments.hpp"

Arguments::Arguments(int argc, const char *const *argv, std::vector<URL> *urls, std::mutex *mtx)
        : _argc(argc), _argv(argv), _urls(urls), _mtx(mtx) {}

int Arguments::get_max_threads() const {
    return _max_threads;
}

int Arguments::_parse_args() {
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help,h", "show this message and exit")
            ("input_file,i", po::value<std::string>(&_input_file), "(REQUIRED) file with urls")
            ("thread_num,n", po::value<int>(&_max_threads)->default_value(1 << 16), "number of threads")
            ("output_dir,o", po::value<std::string>(&_output_dir)->default_value("output"),
             "destination directory");

    po::variables_map vm;
    po::store(po::parse_command_line(_argc, _argv, desc), vm);
    po::notify(vm);

    if (vm.count("help") || !vm.count("input_file")) {
        std::cout << desc;
        return 1;
    }

    if (_max_threads < 1) {
        puts("[Error] Argument thread_num must be positive integer or empty");
        return 2;
    }

    return 0;
}

int Arguments::_manage_files() {
    if (_input_file == "")
        return 0;

    if (!fs::is_regular_file(_input_file)) {
        printf("[Error] No such file: %s\n", _input_file.c_str());
        return 4;
    }

    if (!fs::is_directory(_output_dir)) {
        try {
            return !fs::create_directories(_output_dir);
        } catch (fs::filesystem_error) {
            printf("[Error] Couldn't create directory: %s\n", _output_dir.c_str());
            return 8;
        }
    }

    return 0;
}

int Arguments::_read_urls() {
    std::ifstream file(_input_file);
    std::string url, fname;

    std::lock_guard<std::mutex> lk(*_mtx);
    auto url_id = 0;
    // TODO: read whole lines
    while (file >> url >> fname) {
        if (url[0] == '#')
            continue;

        auto path = _output_dir + "/" + fname;
        _urls->push_back(URL{url_id++, url, path, _mtx});
    }

    return 0;
}

int Arguments::is_valid() {
    return _parse_args() + _manage_files() + _read_urls();
}


