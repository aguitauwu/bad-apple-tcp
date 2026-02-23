#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT     7777
#define ACCSIZE  1048576
#define RECVSIZE 65536

void render(const char *frame, int cols, int rows) {
    printf("\033c");
    int row = 0;
    int col = 0;
    for (const char *p = frame; *p && row < rows; p++) {
        if (*p == '\n') {
            printf("\033[0m\n");
            row++;
            col = 0;
        } else if (col < cols) {
            if (*p == ' ')
                printf("\033[42m \033[0m");
            else
                printf("\033[41m \033[0m");
            col++;
        }
    }
    fflush(stdout);
}

int main(int argc, char *argv[]) {
    const char *host = argc > 1 ? argv[1] : "127.0.0.1";

    int cols = 80, rows = 30;
    const char *cols_env = getenv("COLUMNS");
    const char *rows_env = getenv("LINES");
    if (cols_env) cols = atoi(cols_env);
    if (rows_env) rows = atoi(rows_env) - 1;

    int s = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(PORT);
    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
        perror("inet_pton"); return 1;
    }
    if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect"); return 1;
    }

    printf("Connected. Receiving Bad Apple!!\n");

    char *acc      = calloc(1, ACCSIZE);
    char *recv_buf = malloc(RECVSIZE);
    int acc_len = 0;
    int n;

    while ((n = recv(s, recv_buf, RECVSIZE - 1, 0)) > 0) {
        if (acc_len + n >= ACCSIZE) acc_len = 0;
        memcpy(acc + acc_len, recv_buf, n);
        acc_len += n;
        acc[acc_len] = '\0';

        char *delim;
        while ((delim = strchr(acc, '\f')) != NULL) {
            *delim = '\0';
            render(acc, cols, rows);
            int remaining = acc_len - (int)(delim - acc + 1);
            memmove(acc, delim + 1, remaining);
            acc_len = remaining;
            acc[acc_len] = '\0';
        }
    }

    printf("\033c");
    printf("Bad Apple!! done.\n");
    close(s);
    free(acc);
    free(recv_buf);
    return 0;
}
