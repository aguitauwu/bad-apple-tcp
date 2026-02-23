#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 7777
#define MAX_FILE 67108864
#define MAX_FRAMES 4096

void sleep_ms(int ms) {
    struct timespec ts;
    ts.tv_sec  = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

int main(int argc, char *argv[]) {
    const char *path = argc > 1 ? argv[1] : "play.txt";
    int fps = argc > 2 ? atoi(argv[2]) : 10;

    FILE *f = fopen(path, "r");
    if (!f) { perror("fopen"); return 1; }
    char *raw = malloc(MAX_FILE);
    size_t len = fread(raw, 1, MAX_FILE - 1, f);
    raw[len] = '\0';
    fclose(f);

    for (size_t i = 0; i < len; i++)
        if (raw[i] == '.') raw[i] = ' ';

    char **frames = malloc(sizeof(char*) * MAX_FRAMES);
    int nframes = 0;
    char *p = raw;
    char *found;
    while ((found = strstr(p, "SPLIT")) != NULL && nframes < MAX_FRAMES) {
        *found = '\0';
        if (strlen(p) > 5) frames[nframes++] = p;
        p = found + 5;
    }
    if (*p && nframes < MAX_FRAMES) frames[nframes++] = p;

    printf("%d frames loaded. Listening on port %d...\n", nframes, PORT);

    int srv = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(srv, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); return 1;
    }
    listen(srv, 1);

    int conn = accept(srv, NULL, NULL);
    printf("Client connected. Streaming...\n");

    int delay_ms = 1000 / fps;
    for (int i = 0; i < nframes; i++) {
        size_t flen = strlen(frames[i]);
        send(conn, frames[i], flen, 0);
        send(conn, "\f", 1, 0);
        sleep_ms(delay_ms);
    }

    close(conn);
    close(srv);
    free(raw);
    free(frames);
    return 0;
}
