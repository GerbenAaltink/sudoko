#ifndef FOOTER_H
#define FOOTER_H
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <rlib.h>
#include <pthread.h>
#include <stdarg.h>

static pthread_mutex_t footer_mutex;

static nsecs_t footer_time_start = 0;

char footer_prefix[1024] = {0};
char footer_suffix[1024] = {0};

static void footer_get_terminal_size(int *rows, int *cols) {
    struct winsize w;

    // Use ioctl to get the terminal size
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        perror("ioctl");
    } else {
        *rows = w.ws_row;
        *cols = w.ws_col;
    }
}
void footer_get_cursor_position(int *rows, int *cols) {
    // Save the current terminal settings
    struct termios term, term_saved;
    tcgetattr(STDIN_FILENO, &term);
    term_saved = term;

    // Put terminal in raw mode (disable canonical input and echoing)
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);

    // Request cursor position with ANSI code: ESC [ 6 n
    printf("\033[6n");
    fflush(stdout);

    // Read the response: ESC [ rows ; cols R
    char buf[32];
    long unsigned int i = 0;
    char c = 0;
    while (c != 'R' && i < sizeof(buf) - 1) {
        __attribute_maybe_unused__ ssize_t bytes_read = read(STDIN_FILENO, &c, 1);
        buf[i++] = c;
    }
    buf[i] = '\0';

    // Parse the response
    if (sscanf(buf, "\033[%d;%dR", rows, cols) != 2) {
        *rows = *cols = -1;  // In case of parsing error
    }

    // Restore the original terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &term_saved);
}

void _footer_set(unsigned int row, unsigned int col_count, char * text,unsigned int original_row,unsigned int original_col) {
    char content[2048] = {0};
    char line[1024];
    memset(line,' ',sizeof(line) - 1);
    char cursor_set_string[2048] = {0};
    sprintf(cursor_set_string,"\e[%d;%dH",row,0);
    sprintf(content,"%s%s%s\033[%u;%uH",cursor_set_string,text,line,original_row,original_col);
    content[col_count + strlen(cursor_set_string) + 1] = 0;
    printf(content);
}
void footer_printf(const char *format, ...) {
    if(footer_time_start == 0)
    {
        // Quick, for the threads come a default value
        footer_time_start = 1;
        pthread_mutex_init(&footer_mutex,NULL);
        footer_time_start = nsecs();
    }
    pthread_mutex_lock(&footer_mutex);
    nsecs_t time_elapsed = nsecs() - footer_time_start;
    int original_row = 0;
    int original_col = 0;
    int row_count = 0;
    int col_count = 0;
    footer_get_cursor_position(&original_row, &original_col);
    footer_get_terminal_size(&row_count, &col_count);
    char text[1024] = {0};
    char full_text[4096] = {0};
    va_list args;
    va_start(args, format);
    vsprintf(text, format, args);
    va_end(args);
    sprintf(full_text,"\r%s - %s%s%s", format_time(time_elapsed), footer_prefix, text, footer_suffix);
    
    _footer_set(row_count,col_count,full_text,original_row,original_col);
    pthread_mutex_unlock(&footer_mutex);
}
#endif