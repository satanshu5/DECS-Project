DECS Project  Multi-tier HTTP Key-Value Server
-------------------------------------------------------------
Build:
  mkdir build && cd build
  cmake ..
  make -j

Run:
  scripts/run_all.sh

Dependencies:
  sudo apt update
  sudo apt install -y \
    build-essential \
    cmake \
    libmysqlclient-dev \
    mysql-server \
    libssl-dev \
    nlohmann-json3-dev \
    libboost-all-dev



Put req : curl -X POST http://127.0.0.1:8080/kv -H "Content-Type: application/json" -d '{"k":1,"v":"hello"}'
curl -X POST http://127.0.0.1:8080/kv -H "Content-Type: application/json" -d '{"k":2,"v":"hello1"}'
curl -X POST http://127.0.0.1:8080/kv -H "Content-Type: application/json" -d '{"k":3,"v":"hello2"}'
curl -X POST http://127.0.0.1:8080/kv -H "Content-Type: application/json" -d '{"k":4,"v":"hello3"}'
curl -X POST http://127.0.0.1:8080/kv -H "Content-Type: application/json" -d '{"k":5,"v":"hello4"}'

Get req : curl -X GET http://127.0.0.1:8080/kv?k=1
Delete req : curl -X DELETE http://127.0.0.1:8080/kv?k=1

/* LOADGEN
Compilation:
  g++ -std=c++17 -O2 loadgen.cpp -o loadgen -lcurl -pthread

Examples:
  ./loadgen --workload putall --threads 8 --duration 30
  ./loadgen --workload getpopular --threads 8 --duration 30
  ./loadgen --workload mixed --threads 16 --duration 60
*/