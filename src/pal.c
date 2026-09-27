#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>

static const char *NAMES[8] = {
    "black", "red", "green", "yellow",
    "blue", "magenta", "cyan", "white"
};

static void query_term_color(int id, char *out, size_t sz) {
    struct termios oldt, newt;
    if (tcgetattr(STDIN_FILENO, &oldt) != 0) {
        snprintf(out, sz, "#------");
        return;
    }
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) != 0) {
        snprintf(out, sz, "#------");
        return;
    }

    printf("\033]4;%d;?\007", id);
    fflush(stdout);

    fd_set set;
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    struct timeval tv = {0, 30000};

    char buf[64];
    size_t len = 0;
    int success = 0;

    if (select(STDIN_FILENO + 1, &set, NULL, NULL, &tv) > 0) {
        while (len < sizeof(buf) - 1) {
            char c;
            if (read(STDIN_FILENO, &c, 1) <= 0) break;
            buf[len++] = c;
            if (c == '\007' || (len >= 2 && buf[len - 2] == '\\' && buf[len - 1] == '\\')) {
                success = 1;
                break;
            }
        }
    }
    buf[len] = '\0';

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    if (success) {
        char *ptr = strstr(buf, "rgb:");
        if (ptr) {
            unsigned int r = 0, g = 0, b = 0;
            if (sscanf(ptr, "rgb:%x/%x/%x", &r, &g, &b) == 3) {
                snprintf(out, sz, "#%02x%02x%02x", (r >> 8) & 0xff, (g >> 8) & 0xff, (b >> 8) & 0xff);
                return;
            }
        }
    }

    snprintf(out, sz, "#------");
}

static void print_dots_row(int offset, int color_enabled) {
    printf("  %-8s", offset == 0 ? "dots" : "dots");
    for (int i = 0; i < 8; i++) {
        if (color_enabled) {
            printf("\033[38;5;%dm● ○ •\033[0m   ", i + offset);
        } else {
            printf("● ○ •   ");
        }
    }
    printf("\n");
}

static void print_color_names(const char *label) {
    printf("  %-8s", label);
    for (int i = 0; i < 8; i++) {
        printf("%-8s", NAMES[i]);
    }
    printf("\n");
}

static void print_hex_row(char hex[16][10], int offset, int color_enabled) {
    printf("  %-8s", "hex");
    for (int i = 0; i < 8; i++) {
        if (color_enabled) {
            printf("\033[2m%-8s\033[0m", hex[i + offset]);
        } else {
            printf("%-8s", hex[i + offset]);
        }
    }
    printf("\n");
}

static void print_contrast_row(int offset, int color_enabled) {
    if (offset == 0) {
        printf("  %-8s", "contrast");
    } else {
        printf("          ");
    }
    for (int i = 0; i < 8; i++) {
        if (color_enabled) {
            printf("\033[48;5;%dm  \033[38;5;15m● \033[38;5;0m●  \033[0m", i + offset);
        } else {
            printf("  ● ●   ");
        }
    }
    printf("\n");
}

int main(int argc, char **argv) {
    int simple = 0, plain = 0;
    int mode_full = 1, mode_mini = 0, mode_bar = 0, mode_hex = 0;
    int show_ansi = 0, show_bright = 0, show_contrast = 0, show_syntax = 0;

    const char *no_color = getenv("NO_COLOR");
    if (no_color && no_color[0] != '\0') plain = 1;
    if (!isatty(STDOUT_FILENO)) simple = 1;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("usage: pal [options] [view...]\n\n"
                   "views:\n"
                   "  mini, dots  compact 2-row dot matrix\n"
                   "  bar         color bar\n"
                   "  hex         raw hex codes\n"
                   "  ansi        ansi 8 colors\n"
                   "  bright      bright 8 colors\n"
                   "  contrast    contrast test blocks\n"
                   "  syntax      syntax sample\n\n"
                   "options:\n"
                   "  -m, --mini     compact dots\n"
                   "  -b, --bar      color bar\n"
                   "  -c, --contrast contrast blocks\n"
                   "  -s, --simple   skip osc queries\n"
                   "  -p, --plain    disable ansi escapes\n"
                   "  -v, --version  print version\n"
                   "  -h, --help     print help\n");
            return 0;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            printf("pal 1.1.0\n");
            return 0;
        } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--simple") == 0) {
            simple = 1;
        } else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--plain") == 0) {
            plain = 1;
        } else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--mini") == 0 || strcmp(argv[i], "mini") == 0 || strcmp(argv[i], "dots") == 0) {
            mode_mini = 1; mode_full = 0;
        } else if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--bar") == 0 || strcmp(argv[i], "bar") == 0) {
            mode_bar = 1; mode_full = 0;
        } else if (strcmp(argv[i], "hex") == 0) {
            mode_hex = 1; mode_full = 0;
        } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--contrast") == 0 || strcmp(argv[i], "contrast") == 0) {
            show_contrast = 1; mode_full = 0;
        } else if (strcmp(argv[i], "ansi") == 0) {
            show_ansi = 1; mode_full = 0;
        } else if (strcmp(argv[i], "bright") == 0) {
            show_bright = 1; mode_full = 0;
        } else if (strcmp(argv[i], "syntax") == 0) {
            show_syntax = 1; mode_full = 0;
        } else {
            fprintf(stderr, "pal: unknown option or view '%s'\n", argv[i]);
            return 1;
        }
    }

    char hex[16][10];
    int need_hex = (mode_full || mode_hex || show_ansi || show_bright);
    for (int i = 0; i < 16; i++) {
        if (!simple && need_hex) {
            query_term_color(i, hex[i], sizeof(hex[i]));
        } else {
            snprintf(hex[i], sizeof(hex[i]), "#------");
        }
    }

    int color = !plain;

    if (mode_hex) {
        for (int i = 0; i < 16; i++) {
            if (color) {
                printf("\033[38;5;%dm%s\033[0m\n", i, hex[i]);
            } else {
                printf("%s\n", hex[i]);
            }
        }
        return 0;
    }

    if (mode_mini) {
        for (int row = 0; row < 2; row++) {
            printf("  ");
            for (int i = 0; i < 8; i++) {
                int c = row * 8 + i;
                if (color) {
                    printf("\033[38;5;%dm● ○ •\033[0m   ", c);
                } else {
                    printf("● ○ •   ");
                }
            }
            printf("\n");
        }
        return 0;
    }

    if (mode_bar) {
        for (int row = 0; row < 2; row++) {
            printf("  ");
            for (int i = 0; i < 8; i++) {
                int c = row * 8 + i;
                if (color) {
                    printf("\033[48;5;%dm        \033[0m", c);
                } else {
                    printf("########");
                }
            }
            printf("\n");
        }
        return 0;
    }

    if (mode_full) {
        show_ansi = show_bright = show_contrast = show_syntax = 1;
        if (color) {
            printf("  \033[2mpal :: terminal palette matrix\033[0m\n\n");
        } else {
            printf("  pal :: terminal palette matrix\n\n");
        }
    }

    if (show_ansi) {
        print_color_names("ansi");
        print_dots_row(0, color);
        print_hex_row(hex, 0, color);
        if (show_bright || show_contrast || show_syntax) printf("\n");
    }

    if (show_bright) {
        print_color_names("bright");
        print_dots_row(8, color);
        print_hex_row(hex, 8, color);
        if (show_contrast || show_syntax) printf("\n");
    }

    if (show_contrast) {
        print_contrast_row(0, color);
        print_contrast_row(8, color);
        if (show_syntax) printf("\n");
    }

    if (show_syntax) {
        if (color) {
            printf("  %-8s\033[35mfn\033[0m \033[34mmain\033[0m() { \033[32mprintln!\033[0m(\033[31m\"hello %%s\"\033[0m, \033[33m\"world\"\033[0m); \033[36m// ok\033[0m }\n", "syntax");
        } else {
            printf("  %-8sfn main() { println!(\"hello %%s\", \"world\"); // ok }\n", "syntax");
        }
    }

    return 0;
}
