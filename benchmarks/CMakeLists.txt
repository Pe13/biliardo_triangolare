cmake_minimum_required(VERSION 3.22)
project(benchmarks)

# richiedi l'uso di C++17, senza estensioni non-standard offerte dal compilatore usato
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

# abilita warning
string(APPEND CMAKE_CXX_FLAGS " -Wall -Wextra")

find_package(benchmark REQUIRED)
find_package(Boost 1.35 COMPONENTS container REQUIRED)
find_package(ROOT 6.26 COMPONENTS CONFIG REQUIRED)

add_executable(biliardoBenchmark
        biliardoBenchmarks/fillingBenchmarks.cpp
        biliardoBenchmarks/LaunchingBenchmarks.cpp
        ../Biliardo.cpp)

target_include_directories(biliardoBenchmark PRIVATE ../include)
target_link_libraries(biliardoBenchmark PRIVATE ROOT::Core ROOT::Hist benchmark::benchmark)