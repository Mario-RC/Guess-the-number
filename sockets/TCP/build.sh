#!/bin/sh

gcc -c client.c server.c

if [ -f client ]; then
  rm client
fi
gcc -o client client.c

if [ -f server ]; then
  rm server
fi
gcc -o server server.c

gnome-terminal -- ./server &
sleep 0.5 # Espera 500 ms a que el servidor se active primero
#gnome-terminal -- ./client 127.0.0.1 5000
gnome-terminal -- ./client &
#exit

"$@"
exec "$SHELL"
