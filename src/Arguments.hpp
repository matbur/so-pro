#pragma once

#include "common.hpp"
#include "URL.hpp"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

class Arguments {
public:
    Arguments(int argc, const char *const *argv, std::vector<URL> *urls, std::mutex *mtx);

    int get_max_threads() const;

    int is_valid();

private:
    std::string _input_file;
    int _max_threads;
    std::string _output_dir;
    int _argc;
    const char *const *_argv;
    std::vector<URL> *_urls;
    std::mutex *_mtx;

    int _parse_args();

    int _manage_files();

    int _read_urls();
};
