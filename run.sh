#!/bin/sh

if [ "$1" = "client" ] || [ "$1" = "c" ]; then
    ./bin/client
elif [ "$1" = "server" ] || [ "$1" = "s" ]; then
    ./bin/server
else
    echo "Usage: ./run.sh [client|server]"
    echo "Invalid argument: $1"
    echo "Usage: $0 [client|server]"
    exit 1
fi