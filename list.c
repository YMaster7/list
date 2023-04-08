#include <dirent.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <linux/limits.h>

void path_join(char *buf, const char *path1, const char *path2) {
    strcpy(buf, path1);
    if (path1[strlen(path1) - 1] != '/') {
        strcat(buf, "/");
    }
    strcat(buf, path2);
}

// Convert a size string to bytes
long long parse_size(const char *size_str) {
    long long size = 0;
    char unit = 'B';
    sscanf(size_str, "%lld%c", &size, &unit);
    switch (unit) {
        case 'B':
            break;
        case 'K':
            size *= 1024;
            break;
        case 'M':
            size *= 1024 * 1024;
            break;
        case 'G':
            size *= 1024 * 1024 * 1024;
            break;
        default:
            fprintf(stderr, "Invalid size unit: %c (must be B, K, M, or G)\n",
                    unit);
            exit(EXIT_FAILURE);
    }
    return size;
}

// Convert a size in bytes to a string
const char *format_size(long long size) {
    static char size_str[32];
    char *unit[] = {"B", "K", "M", "G"};
    int i = 0;
    double size_d = size;
    while (size_d >= 1024 && i < 3) {
        size_d /= 1024;
        i++;
    }
    sprintf(size_str, "%.1f%s", size_d, unit[i]);
    return size_str;
}

void print_file(struct stat st, const char *path, long long min_size,
                long long max_size, int n_days) {
    if (min_size >= 0 && st.st_size < min_size) {
        return;
    }
    if (max_size >= 0 && st.st_size > max_size) {
        return;
    }
    if (n_days >= 0) {
        time_t now = time(NULL);
        if (difftime(now, st.st_mtime) > n_days * 24 * 60 * 60) {
            return;
        }
    }
    
    static char mtime_str[32];
    strftime(mtime_str, sizeof(mtime_str), "%Y-%m-%d %H:%M",
             localtime(&st.st_mtime));
    char dir_indicator = S_ISDIR(st.st_mode) ? '/' : ' ';
    printf("%s\t%s\t%s%c\n", mtime_str, format_size(st.st_size), path,
           dir_indicator);
}

void list(const char *path, int recursive, int all, long long min_size,
          long long max_size, int n_days) {
    static struct stat st;
    char full_path[PATH_MAX];

    if (stat(path, &st) == -1) {
        perror(path);
        return;
    }

    if (!S_ISDIR(st.st_mode)) {
        print_file(st, path, min_size, max_size, n_days);
    } else {
        DIR *dir = opendir(path);
        if (dir == NULL) {
            perror(path);
            return;
        }

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (!all && entry->d_name[0] == '.') {
                continue;
            }
            
            path_join(full_path, path, entry->d_name);

            if (stat(full_path, &st) == -1) {
                perror(full_path);
                return;
            }

            print_file(st, full_path, min_size, max_size, n_days);
            if (S_ISDIR(st.st_mode) && recursive &&
                strcmp(entry->d_name, ".") != 0 &&
                strcmp(entry->d_name, "..") != 0) {
                list(full_path, recursive, all, min_size, max_size, n_days);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    // Default options
    int recursive = 0, all = 0;
    long long min_size = -1, max_size = -1;
    int n_days = -1;

    // Parse options
    int opt;
    while ((opt = getopt(argc, argv, "ral:h:m:")) != -1) {
        switch (opt) {
            case 'r':
                recursive = 1;
                break;
            case 'a':
                all = 1;
                break;
            case 'l':
                min_size = parse_size(optarg);
                break;
            case 'h':
                max_size = parse_size(optarg);
                break;
            case 'm':
                n_days = atoi(optarg);
                break;
            default:
                fprintf(stderr,
                        "Usage: %s [-r] [-a] [-l MIN_SIZE] [-h MAX_SIZE] [-m "
                        "N_DAYS] [FILE/DIR]\n",
                        argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    int num_args = argc - optind;
    if (num_args == 0) {
        list(".", recursive, all, min_size, max_size, n_days);
    } else {
        for (int i = optind; i < argc; i++) {
            list(argv[i], recursive, all, min_size, max_size, n_days);
        }
    }

    return 0;
}
