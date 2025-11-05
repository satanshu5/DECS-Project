cmake_minimum_required(VERSION 3.10)
project(kv_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(Threads REQUIRED)

# ------------------------------------------------------------------
# Try to find Boost
# ------------------------------------------------------------------
find_package(Boost COMPONENTS thread system REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# ------------------------------------------------------------------
# Include paths
# ------------------------------------------------------------------
include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/third_party
)

# ------------------------------------------------------------------
# Server executable (Phase 1)
# ------------------------------------------------------------------
add_executable(kv_server
  ${CMAKE_SOURCE_DIR}/src/main.cpp
  ${CMAKE_SOURCE_DIR}/src/server.cpp
  ${CMAKE_SOURCE_DIR}/src/db.cpp
)

# ------------------------------------------------------------------
# MySQL linking
# ------------------------------------------------------------------
find_library(MYSQLCLIENT_LIB mysqlclient)
if(NOT MYSQLCLIENT_LIB)
  message(FATAL_ERROR "mysqlclient library not found. Install libmysqlclient-dev.")
endif()

# ------------------------------------------------------------------
# Link everything together
# ------------------------------------------------------------------
target_link_libraries(kv_server
  PRIVATE
    ${MYSQLCLIENT_LIB}
    ${Boost_LIBRARIES}
    Threads::Threads
)

message(STATUS "Build configured successfully.")
message(STATUS "  Boost include: ${Boost_INCLUDE_DIRS}")
message(STATUS "  Boost libs: ${Boost_LIBRARIES}")
