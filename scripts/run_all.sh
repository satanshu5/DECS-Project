#!/bin/bash

set -e

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------
DB_NAME="kvdb"
DB_USER="kvuser"
DB_PASS="kvpass"
PORT=8080
WORKERS=8
CACHE_SIZE=10000

# ---------------------------------------------------------------------------
# Start MySQL
# ---------------------------------------------------------------------------
echo "Starting MySQL service."
sudo service mysql start

# ---------------------------------------------------------------------------
# Initialize database and user
# ---------------------------------------------------------------------------
echo "Initializing MySQL database and user..."
if [ -f "scripts/init_db.sh" ]; then
    chmod +x scripts/init_db.sh
    ./scripts/init_db.sh
else
    echo "scripts/init_db.sh not found! Please run the command from root."
    exit 1
fi

# ---------------------------------------------------------------------------
# Export DB credentials (so server connects as kvuser)
# ---------------------------------------------------------------------------
export KV_DB_USER=$DB_USER
export KV_DB_PASS=$DB_PASS
echo "Exported KV_DB_USER=$KV_DB_USER"
echo "Exported KV_DB_PASS=$KV_DB_PASS"

# ---------------------------------------------------------------------------
# Build project (CMake + Make)
# ---------------------------------------------------------------------------
echo "Building C++ project..."
if [ ! -d "build" ]; then
    mkdir build
fi

cd build
cmake .. >/dev/null
make -j >/dev/null
cd ..

# ---------------------------------------------------------------------------
# Launch the server
# ---------------------------------------------------------------------------
echo "Launching key-value server..."
echo "Port: $PORT "
echo "Workers: $WORKERS"
echo "Cache size: $CACHE_SIZE"
echo "--------------------------------------------"
echo "Server logs below:"
echo
./build/kv_server $PORT $WORKERS $CACHE_SIZE
