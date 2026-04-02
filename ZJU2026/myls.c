#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

/*
 * 一个加强版 ls：
 *   支持：
 *     -a  显示隐藏文件
 *     -l  长格式
 *     -R  递归列出子目录
 *     -t  按修改时间排序
 *     -r  反向排序
 *
 * 说明：
 *   1. 这是 POSIX C 程序，不是纯 ISO C。
 *   2. 为了教学清晰，代码尽量写得直白。
 *   3. 不追求与 GNU ls 完全一致，但核心行为接近。
 */

typedef struct {
    int show_all;   /* -a */
    int long_fmt;   /* -l */
    int recursive;  /* -R */
    int sort_time;  /* -t */
    int reverse;    /* -r */
} Options;

/* 目录中的一个条目 */
typedef struct {
    char *name;        /* 文件名，不含父目录 */
    char *fullpath;    /* 完整路径 */
    struct stat st;    /* 文件状态 */
    int stat_ok;       /* lstat 是否成功 */
} Entry;

/* Entry 动态数组 */
typedef struct {
    Entry *data;
    size_t size;
    size_t capacity;
} EntryArray;

/* ---------- 工具函数：错误退出 ---------- */
static void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

/* ---------- 安全 strdup ---------- */
static char *xstrdup(const char *s) {
    char *p = strdup(s);
    if (!p) {
        die("strdup");
    }
    return p;
}

/* ---------- 初始化 EntryArray ---------- */
static void ea_init(EntryArray *a) {
    a->data = NULL;
    a->size = 0;
    a->capacity = 0;
}

/* ---------- 向 EntryArray 追加一个元素 ---------- */
static void ea_push(EntryArray *a, const Entry *e) {
    if (a->size == a->capacity) {
        size_t new_cap = (a->capacity == 0) ? 16 : a->capacity * 2;
        Entry *new_data = realloc(a->data, new_cap * sizeof(Entry));
        if (!new_data) {
            die("realloc");
        }
        a->data = new_data;
        a->capacity = new_cap;
    }
    a->data[a->size++] = *e;
}

/* ---------- 释放 EntryArray ---------- */
static void ea_free(EntryArray *a) {
    size_t i;
    for (i = 0; i < a->size; ++i) {
        free(a->data[i].name);
        free(a->data[i].fullpath);
    }
    free(a->data);
    a->data = NULL;
    a->size = 0;
    a->capacity = 0;
}

/* ---------- 反转数组 ---------- */
static void ea_reverse(EntryArray *a) {
    size_t i = 0;
    size_t j = (a->size == 0) ? 0 : a->size - 1;
    while (i < j) {
        Entry tmp = a->data[i];
        a->data[i] = a->data[j];
        a->data[j] = tmp;
        ++i;
        --j;
    }
}

/* ---------- 路径拼接：dir + "/" + name ---------- */
static char *join_path(const char *dir, const char *name) {
    size_t len1 = strlen(dir);
    size_t len2 = strlen(name);
    int need_slash = (len1 > 0 && dir[len1 - 1] != '/');

    size_t total = len1 + (need_slash ? 1 : 0) + len2 + 1;
    char *result = malloc(total);
    if (!result) {
        die("malloc");
    }

    strcpy(result, dir);
    if (need_slash) {
        strcat(result, "/");
    }
    strcat(result, name);

    return result;
}

/* ---------- 简单 basename：取最后一个 / 后面的部分 ---------- */
static const char *base_name_simple(const char *path) {
    const char *p = strrchr(path, '/');
    return p ? p + 1 : path;
}

/* ---------- 把 mode 转成 ls -l 风格权限串 ---------- */
static void mode_to_string(mode_t mode, char out[11]) {
    out[0] = S_ISDIR(mode)  ? 'd' :
             S_ISLNK(mode)  ? 'l' :
             S_ISCHR(mode)  ? 'c' :
             S_ISBLK(mode)  ? 'b' :
             S_ISFIFO(mode) ? 'p' :
             S_ISSOCK(mode) ? 's' : '-';

    out[1] = (mode & S_IRUSR) ? 'r' : '-';
    out[2] = (mode & S_IWUSR) ? 'w' : '-';
    out[3] = (mode & S_IXUSR) ? 'x' : '-';

    out[4] = (mode & S_IRGRP) ? 'r' : '-';
    out[5] = (mode & S_IWGRP) ? 'w' : '-';
    out[6] = (mode & S_IXGRP) ? 'x' : '-';

    out[7] = (mode & S_IROTH) ? 'r' : '-';
    out[8] = (mode & S_IWOTH) ? 'w' : '-';
    out[9] = (mode & S_IXOTH) ? 'x' : '-';

    out[10] = '\0';
}

/* ---------- 格式化修改时间 ---------- */
static void format_mtime(time_t t, char *buf, size_t bufsize) {
    struct tm *tm_info = localtime(&t);
    if (!tm_info) {
        snprintf(buf, bufsize, "????????????");
        return;
    }
    strftime(buf, bufsize, "%b %d %H:%M", tm_info);
}

/* ---------- 默认按名字排序 ---------- */
static int cmp_entry_name(const void *p1, const void *p2) {
    const Entry *e1 = (const Entry *)p1;
    const Entry *e2 = (const Entry *)p2;
    return strcmp(e1->name, e2->name);
}

/* ---------- 按时间排序：新的在前；时间相同则按名字 ---------- */
static int cmp_entry_time(const void *p1, const void *p2) {
    const Entry *e1 = (const Entry *)p1;
    const Entry *e2 = (const Entry *)p2;

    time_t t1 = e1->stat_ok ? e1->st.st_mtime : 0;
    time_t t2 = e2->stat_ok ? e2->st.st_mtime : 0;

    if (t1 > t2) return -1;
    if (t1 < t2) return 1;

    return strcmp(e1->name, e2->name);
}

/* ---------- 打印单个条目 ---------- */
static void print_entry(const Entry *e, const Options *opt) {
    if (!e->stat_ok) {
        fprintf(stderr, "ls: cannot access '%s': %s\n", e->fullpath, strerror(errno));
        return;
    }

    if (!opt->long_fmt) {
        printf("%s\n", e->name);
        return;
    }

    char mode_str[11];
    char time_str[64];

    mode_to_string(e->st.st_mode, mode_str);
    format_mtime(e->st.st_mtime, time_str, sizeof(time_str));

    struct passwd *pw = getpwuid(e->st.st_uid);
    const char *user = pw ? pw->pw_name : "?";

    struct group *gr = getgrgid(e->st.st_gid);
    const char *group = gr ? gr->gr_name : "?";

    printf("%s %2lu %-8s %-8s %8lld %s %s",
           mode_str,
           (unsigned long)e->st.st_nlink,
           user,
           group,
           (long long)e->st.st_size,
           time_str,
           e->name);

    /* 若是符号链接，显示 -> 目标 */
    if (S_ISLNK(e->st.st_mode)) {
        char buf[4096];
        ssize_t n = readlink(e->fullpath, buf, sizeof(buf) - 1);
        if (n >= 0) {
            buf[n] = '\0';
            printf(" -> %s", buf);
        }
    }

    putchar('\n');
}

/* ---------- 读取目录内容到数组 ---------- */
static void read_directory_entries(const char *path, const Options *opt, EntryArray *entries) {
    DIR *dir = opendir(path);
    if (!dir) {
        fprintf(stderr, "ls: cannot open directory '%s': %s\n", path, strerror(errno));
        return;
    }

    for (;;) {
        errno = 0;
        struct dirent *ent = readdir(dir);
        if (!ent) {
            if (errno != 0) {
                fprintf(stderr, "ls: readdir failed for '%s': %s\n", path, strerror(errno));
            }
            break;
        }

        /* 默认不显示隐藏文件 */
        if (!opt->show_all && ent->d_name[0] == '.') {
            continue;
        }

        Entry e;
        e.name = xstrdup(ent->d_name);
        e.fullpath = join_path(path, ent->d_name);
        e.stat_ok = (lstat(e.fullpath, &e.st) == 0);

        ea_push(entries, &e);
    }

    closedir(dir);
}

/* ---------- 排序 ---------- */
static void sort_entries(EntryArray *entries, const Options *opt) {
    if (opt->sort_time) {
        qsort(entries->data, entries->size, sizeof(Entry), cmp_entry_time);
    } else {
        qsort(entries->data, entries->size, sizeof(Entry), cmp_entry_name);
    }

    if (opt->reverse) {
        ea_reverse(entries);
    }
}

/* ---------- 列出一个目录 ---------- */
static void list_directory(const char *path, const Options *opt, int print_header) {
    EntryArray entries;
    ea_init(&entries);

    read_directory_entries(path, opt, &entries);
    sort_entries(&entries, opt);

    if (print_header) {
        printf("%s:\n", path);
    }

    for (size_t i = 0; i < entries.size; ++i) {
        print_entry(&entries.data[i], opt);
    }

    /* -R：递归进入子目录 */
    if (opt->recursive) {
        for (size_t i = 0; i < entries.size; ++i) {
            Entry *e = &entries.data[i];

            if (!e->stat_ok) {
                continue;
            }

            /* 只递归真正的目录，不递归符号链接 */
            if (!S_ISDIR(e->st.st_mode)) {
                continue;
            }

            /* 必须跳过 . 和 ..，否则会无限递归 */
            if (strcmp(e->name, ".") == 0 || strcmp(e->name, "..") == 0) {
                continue;
            }

            printf("\n%s:\n", e->fullpath);
            list_directory(e->fullpath, opt, 0);
        }
    }

    ea_free(&entries);
}

/* ---------- 处理单个路径：可能是文件，也可能是目录 ---------- */
static void list_path(const char *path, const Options *opt, int print_header_for_dir) {
    struct stat st;

    if (lstat(path, &st) != 0) {
        fprintf(stderr, "ls: cannot access '%s': %s\n", path, strerror(errno));
        return;
    }

    if (S_ISDIR(st.st_mode)) {
        list_directory(path, opt, print_header_for_dir);
    } else {
        Entry e;
        e.name = xstrdup(base_name_simple(path));
        e.fullpath = xstrdup(path);
        e.st = st;
        e.stat_ok = 1;

        print_entry(&e, opt);

        free(e.name);
        free(e.fullpath);
    }
}

/* ---------- 解析选项 ---------- */
static int parse_options(int argc, char *argv[], Options *opt, int *first_path_index) {
    int i;

    opt->show_all = 0;
    opt->long_fmt = 0;
    opt->recursive = 0;
    opt->sort_time = 0;
    opt->reverse = 0;

    for (i = 1; i < argc; ++i) {
        if (argv[i][0] != '-' || argv[i][1] == '\0') {
            break;
        }

        if (strcmp(argv[i], "--") == 0) {
            ++i;
            break;
        }

        for (int j = 1; argv[i][j] != '\0'; ++j) {
            switch (argv[i][j]) {
                case 'a':
                    opt->show_all = 1;
                    break;
                case 'l':
                    opt->long_fmt = 1;
                    break;
                case 'R':
                    opt->recursive = 1;
                    break;
                case 't':
                    opt->sort_time = 1;
                    break;
                case 'r':
                    opt->reverse = 1;
                    break;
                default:
                    fprintf(stderr, "Usage: %s [-a] [-l] [-R] [-t] [-r] [file ...]\n", argv[0]);
                    return -1;
            }
        }
    }

    *first_path_index = i;
    return 0;
}

int main(int argc, char *argv[]) {
    Options opt;
    int first_path_index;

    if (parse_options(argc, argv, &opt, &first_path_index) != 0) {
        return EXIT_FAILURE;
    }

    /* 默认列当前目录 */
    if (first_path_index >= argc) {
        list_path(".", &opt, 0);
        return EXIT_SUCCESS;
    }

    int path_count = argc - first_path_index;

    for (int i = first_path_index; i < argc; ++i) {
        struct stat st;
        int is_dir = (lstat(argv[i], &st) == 0 && S_ISDIR(st.st_mode));
        int print_header = (path_count > 1 && is_dir);

        list_path(argv[i], &opt, print_header);

        if (i + 1 < argc) {
            putchar('\n');
        }
    }

    return EXIT_SUCCESS;
}