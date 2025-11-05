DECS Project  Multi-tier HTTP Key-Value Server
-------------------------------------------------------------
Build:
  mkdir build && cd build
  cmake ..
  make -j

Run:
  scripts/run_all.sh

Dependencies:
  sudo apt install build-essential cmake libmysqlclient-dev mysql-server

Workloads supported:
  put_all       → DB write-heavy (disk-bound)
  get_all_unique→ DB read-heavy (disk-bound)
  get_popular   → Cache-heavy (CPU-bound)

Put req : curl -X POST http://127.0.0.1:8080/kv -H "Content-Type: application/json" -d '{"k":1,"v":"hello"}'

Get req : curl -X GET http://127.0.0.1:8080/kv?k=1
Delete req : curl -X DELETE http://127.0.0.1:8080/kv?k=1