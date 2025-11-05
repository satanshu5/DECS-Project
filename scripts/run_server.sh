#!/bin/bash
# usage: ./run_server.sh [port] [cache_size]
PORT=${1:-8080}
CACHE_SIZE=${2:-10000}
./kv_server ${PORT} 8 ${CACHE_SIZE}
