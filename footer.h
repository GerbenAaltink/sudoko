#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <rlib.h>
#include <pthread.h>

pthread_mutex_t footer_mutex;

nsecs_t footer_time_start = 0;

char footer_prefix[1024] = {0};
char footer_buffer[1024] = {0};
char footer_suffix[1024] = {0};

void get_terminal_size(int *rows, int *cols) {
    struct winsize w;

    // Use ioctl to get the terminal size
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        perror("ioctl");
    } else {
        *rows = w.ws_row;
        *cols = w.ws_col;
    }
}
void get_cursor_position(int *rows, int *cols) {
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
    int i = 0;
    char c = 0;
    while (c != 'R' && i < sizeof(buf) - 1) {
        read(STDIN_FILENO, &c, 1);
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

void set_cursor_position(int row, int col) {
    // Use ANSI escape code to set the cursor position
    printf("\033[%d;%dH", row, col);
    fflush(stdout); // Ensure the output is flushed to the terminal
}
void _set_footer(unsigned int row, unsigned int col_count, char * text,unsigned int original_row,unsigned int original_col) {
    char line[col_count];
    memset(line,' ',col_count);
    line[col_count - strlen(text)] = 0;
    printf("\033[%d;%dH%s%s\033[%ld;%ldH",row,0,text,line,original_row,original_col);
}
void set_footer_text(const char *text) {
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
    get_cursor_position(&original_row, &original_col);
    get_terminal_size(&row_count, &col_count);
    //set_cursor_position(row_count, 0);
    char full_text[1024 * 3 + 1] = {0};
    sprintf(full_text,"\rduration: %s - %s%s%s", format_time(time_elapsed), footer_prefix, text, footer_suffix);
    _set_footer(row_count,col_count,full_text,original_row,original_col);
    //memset(footer_buffer,0,sizeof(footer_buffer));
    pthread_mutex_unlock(&footer_mutex);
}