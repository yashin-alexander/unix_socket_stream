### UNIX SOCK_STREAM sockets demo app
This app analyzing input stream as an int16_t numbers, then it's trying to find local maximums in that stream.
To start maximums lookup process app will firstly accumulate 10001 16-bit integers, so you'll need to pass some data inside(just redirect 20002 bytes from `/dev/urandom` or `/dev/zero`).

I've also added a systemd service file example, so this app may be installed to stay persistent after system power-up.
But it's also depends on your distro whether this target will be suitable for you(as the directories structure may differ from distro to distro), so it may be used mainly for reference\example.

To run:
```shell
make
./build/unix_socket_stream

head -n 100 /dev/urandom > input.txt
socat UNIX-CONNECT:/tmp/example - < input.txt
cat /tmp/example.log
```
