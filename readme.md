# Bad Apple!! over TCP in C

Bad Apple!! streamed over raw TCP sockets written in pure C.
No libraries. No frameworks. Just sockets.

## How it works

The server reads the ASCII frames from `play.txt` and streams them
over TCP one frame at a time at 10fps. The client receives each frame
and renders it in the terminal using ANSI color codes — red for dark
pixels, green for light pixels.

## Requirements

- C compiler (clang or gcc)
- Two terminals (or two machines on the same network)

## Build

```bash
clang server.c -o server
clang client.c -o client
```

## Run

Terminal 1:
```bash
./server play.txt
```

Terminal 2:
```bash
COLUMNS=100 LINES=35 ./client 127.0.0.1
```

To watch it from another machine on the network, replace `127.0.0.1`
with the server's IP address.

## Watch the traffic

If you want to see Bad Apple!! in the raw packets:

```bash
tcpdump -i lo -A port 7777
```

## Files

| File | Description |
|---|---|
| `server.c` | Reads frames and streams them over TCP |
| `client.c` | Receives frames and renders them in the terminal |
| `play.txt` | ASCII frames source |

## License

MIT — aguitauwu

