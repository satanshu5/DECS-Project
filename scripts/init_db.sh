#!/bin/bash
# ---------------------------------------------------------------------------
# Initialize MySQL database and user for DECS KV project
# Usage: ./scripts/init_db.sh
# ---------------------------------------------------------------------------

set -e

DB_NAME="kvdb"
DB_USER="kvuser"
DB_PASS="kvpass"

echo "Starting MySQL service (if not already running)..."
sudo service mysql start

echo "Creating database '$DB_NAME' and user '$DB_USER'..."

sudo mysql <<SQL
CREATE DATABASE IF NOT EXISTS ${DB_NAME};
CREATE USER IF NOT EXISTS '${DB_USER}'@'localhost' IDENTIFIED BY '${DB_PASS}';
GRANT ALL PRIVILEGES ON ${DB_NAME}.* TO '${DB_USER}'@'localhost';
FLUSH PRIVILEGES;

USE ${DB_NAME};
CREATE TABLE IF NOT EXISTS kv_table (
  k INT PRIMARY KEY,
  v TEXT NOT NULL,
  updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);
SQL


