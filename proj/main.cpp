#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <curl/curl.h>
#include <ncurses.h>
#include <list>
#include <deque>
#include <string.h>

using namespace std;

struct file_t {
    int id;
    char url[1000];
    double total;
    double downloaded;
    int thread;
    bool done;
    char path[1000];
    bool error;
    char error_msg[CURL_ERROR_SIZE];
};

deque<file_t *> files_queue;
pthread_mutex_t files_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
list<file_t *> files;
pthread_t *worker_threads;
int *worker_threads_args;
int worker_threads_count;
bool worker_threads_running = true;
int last_file_id = -1;

pthread_t gui_thread;
bool gui_thread_keep = true;
bool gui_changed = false;
int gui_offset_x = 0;
int gui_offset_y = 0;
int screen_width;
int screen_height;
bool gui_autoscroll = true;

pthread_mutex_t gui_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t gui_cond = PTHREAD_COND_INITIALIZER;

char *get_filename(char *str) {
    char *name;
    int len = strlen(str);
    int i, k;
    for (i = len - 1; i >= 0; i--) {
        if (str[i] == '/') {
            name = new char[len - i];
            for (k = 0; k < (len - i); k++) name[k] = str[k + i + 1];
            name[k] = '\0';
            return name;
        }
    }
    return NULL;
}

void *download_progress_func(file_t *f, double t, double d, double ultotal, double ulnow) {
    pthread_mutex_lock(&gui_mutex);
    f->total = t;
    f->downloaded = d;
    gui_changed = true;
    pthread_cond_signal(&gui_cond);
    pthread_mutex_unlock(&gui_mutex);

    return NULL;
}

void *worker_thread_fun(void *arg) {
    int wt = *((int *) arg);
    file_t *f;
    char *err;

    while (true) {
        pthread_mutex_lock(&files_queue_mutex);

        if (files_queue.empty()) {
            pthread_mutex_unlock(&files_queue_mutex);
            break;
        }

        f = files_queue.front();
        files_queue.pop_front();
        last_file_id = max(last_file_id, f->id);
        pthread_mutex_unlock(&files_queue_mutex);

        pthread_mutex_lock(&gui_mutex);
        f->thread = wt;
        gui_changed = true;
        pthread_cond_signal(&gui_cond);
        pthread_mutex_unlock(&gui_mutex);

        // Download
        CURL *curl = curl_easy_init();
        if (curl) {
            char tmp_path[40];
            char *full_url = NULL;
            sprintf(tmp_path, "/tmp/download-%d-%d.tmp", getpid(), wt);
            FILE *outfile = fopen(tmp_path, "w");

            curl_easy_setopt(curl, CURLOPT_URL, f->url);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, outfile);
            curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, f->error_msg);

            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
            curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, download_progress_func);
            curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, f);

            CURLcode url_res = curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &full_url);
            char *fname = get_filename(full_url);

            if (fname == NULL) {
                pthread_mutex_lock(&gui_mutex);
                f->thread = -1;
                f->done = true;
                f->error = true;

                strcpy(f->error_msg, "Not a file");

                gui_changed = true;
                pthread_cond_signal(&gui_cond);
                pthread_mutex_unlock(&gui_mutex);
            } else {
                strcpy(f->path, fname);
                delete fname;

                CURLcode res = curl_easy_perform(curl);

                pthread_mutex_lock(&gui_mutex);

                f->thread = -1;
                f->done = true;

                if (res == CURLE_OK) {
                    f->error = false;
                    rename(tmp_path, f->path);
                } else {
                    f->error = true;
                }

                gui_changed = true;
                pthread_cond_signal(&gui_cond);
                pthread_mutex_unlock(&gui_mutex);
            }

            fclose(outfile);
            curl_easy_cleanup(curl);
        } else {
            // printf("CURL FAILED\n");
            // exit(-1);
        }
    }

    return NULL;
}

void init_worker_threads() {
    worker_threads = new pthread_t[worker_threads_count];
    worker_threads_args = new int[worker_threads_count];

    for (int i = 0; i < worker_threads_count; i++) {
        worker_threads_args[i] = i;
        pthread_create(worker_threads + i, NULL, worker_thread_fun, worker_threads_args + i);
    }
}

void wait_for_worker_threads() {
    for (int i = 0; i < worker_threads_count; i++) {
        pthread_join(worker_threads[i], NULL);
    }
    worker_threads_running = false;

    // last gui thread notify
    pthread_mutex_lock(&gui_mutex);
    gui_changed = true;
    pthread_cond_signal(&gui_cond);
    pthread_mutex_unlock(&gui_mutex);
}

int get_done_files_count() {
    int c = 0;
    for (list<file_t *>::iterator it = files.begin(); it != files.end(); it++) {
        if ((*it)->done) c++;
    }
    return c;
}

void print_done() {
    printw("\n\n Download finished. Press any key to exit.");
}

void paint() {
    char str[256];
    char bar[40];
    double perc;
    int x = gui_offset_x;
    int y = 0;
    file_t *f;

    erase();
    mvprintw(0, 0, " == Parallel cURL == (%d threads, %d files, %d downloaded) - autoscroll: %s (press 's' to toggle)",
             worker_threads_count,
             files.size(),
             get_done_files_count(),
             gui_autoscroll ? "on" : "off");

    y += 2;

    int i = -1;
    for (list<file_t *>::iterator it = files.begin(); it != files.end(); it++) {
        i++;
        if (i >= gui_offset_y) {
            f = *it;
            clrtoeol();

            if (f->done) {
                mvprintw(y, x, "%3d. %-50s", i + 1, f->url);
                if (f->error) {
                    attron(COLOR_PAIR(3));
                    mvprintw(y, x + 55, " ERROR: %s", f->error_msg);
                    attroff(COLOR_PAIR(3));
                } else {
                    attron(COLOR_PAIR(2));
                    mvprintw(y, x + 55, " 100%%");
                    attroff(COLOR_PAIR(2));
                    mvprintw(y, x + 61, "%s", f->path);
                }
            } else if (f->thread != -1) {
                if (f->total != 0.0) perc = f->downloaded * 100 / f->total;
                else perc = 0.0;

                int p = (int) (perc / 5);
                for (int j = 0; j < p; j++) bar[j] = '|';
                bar[p] = '\0';

                mvprintw(y, x, "%3d. %-50s [", i + 1, f->url);
                attron(COLOR_PAIR(1));
                mvprintw(y, x + 57, "%-20s", bar);
                attroff(COLOR_PAIR(1));
                mvprintw(y, x + 77, "] - %6.2f%% (Thread #%d)", perc, f->thread);
            } else {
                mvprintw(y, x, "%3d. %s", (i + 1), f->url);
            }
            y++;
        }
    }

    refresh();
}

void *gui_thread_fun(void *_arg) {
    int c;
    initscr();
    keypad(stdscr, true);
    getmaxyx(stdscr, screen_height, screen_width);

    if (!has_colors()) {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }

    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);

    nodelay(stdscr, true);

    while (worker_threads_running) {
        pthread_mutex_lock(&gui_mutex);
        while (!gui_changed) {
            if ((c = getch()) != ERR) {
                switch (c) {
                    case 's':
                        gui_autoscroll = !gui_autoscroll;
                        break;

                    case KEY_UP:
                        if (!gui_autoscroll && gui_offset_y > 0) gui_offset_y--;
                        break;

                    case KEY_DOWN:
                        int df = files.size() + 2 - screen_height;
                        if (!gui_autoscroll && gui_offset_y < df) gui_offset_y++;
                        break;
                }
            }

            pthread_cond_wait(&gui_cond, &gui_mutex);
        }

        if (gui_autoscroll) gui_offset_y = last_file_id - screen_height + 3;

        paint();
        gui_changed = false;
        pthread_mutex_unlock(&gui_mutex);
    }


    nodelay(stdscr, false);

    print_done();
    getch();

    endwin();

    return NULL;
}

void init_gui_thread() {
    pthread_create(&gui_thread, NULL, gui_thread_fun, NULL);
}

void wait_for_gui_thread() {
    gui_thread_keep = false;
    pthread_join(gui_thread, NULL);
}

void cleanup() {
    pthread_cond_destroy(&gui_cond);
    pthread_mutex_destroy(&gui_mutex);
    pthread_mutex_destroy(&files_queue_mutex);

    for (list<file_t *>::iterator it = files.begin(); it != files.end(); it++) {
        delete *it;
    }

    delete[] worker_threads;
    delete[] worker_threads_args;
}

int main(int argc, char const *argv[]) {
    if (argc < 3) {
        printf("Usage: %s [FILE] [WORKERS_NUM]\n", argv[0]);
        exit(-1);
    }

    // Create files queue
    FILE *urls_file = fopen(argv[1], "r");
    if (urls_file != NULL) {
        char url[256];
        file_t *f;
        int i = 0;

        while (fscanf(urls_file, "%s", url) != EOF) {
            if (url[0] == '#') continue;
            f = new file_t;
            strcpy(f->url, url);
            f->id = i++;
            f->thread = -1;
            f->done = false;
            f->error = false;
            files.push_back(f);
            files_queue.push_back(f);
        }

        fclose(urls_file);
    } else {
        printf("File '%s' not found\n", argv[1]);
        exit(-1);
    }

    worker_threads_count = atoi(argv[2]);

    curl_global_init(CURL_GLOBAL_ALL);

    init_gui_thread();
    init_worker_threads();

    wait_for_worker_threads();
    wait_for_gui_thread();

    cleanup();

    return 0;
}
