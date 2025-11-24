#!/bin/bash
# ---------------------------------------------------------------------------
# Flush (empty) MySQL kv_table in kvdb database
# Usage: ./scripts/flush_db.sh
# ---------------------------------------------------------------------------

set -e

DB_NAME="kvdb"
DB_USER="kvuser"
DB_PASS="kvpass"
DB_HOST="localhost"

echo "Starting MySQL service (if not already running)..."
sudo service mysql start

echo "Flushing table 'kv_table' in database '$DB_NAME'..."

mysql -h"$DB_HOST" -u"$DB_USER" -p"$DB_PASS" "$DB_NAME" <<SQL

TRUNCATE TABLE kv_table;

SQL

echo "Flush complete — kv_table is now empty."
