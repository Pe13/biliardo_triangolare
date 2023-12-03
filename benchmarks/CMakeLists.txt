cmake_minimum_required(VERSION 3.22)
project(benchmarks)

# abilita warning
string(APPEND CMAKE_CXX_FLAGS " -Wall -Wextra")

find_package(benchmark REQUIRED)
find_package(Boost 1.35 COMPONENTS container REQUIRED)
find_package(ROOT 6.26 COMPONENTS CONFIG REQUIRED)

add_executable(biliardoBenchmark biliardoBenchmark.cpp ../Biliardo.cpp ../BiliardoFunctions.cpp)

target_include_directories(biliardoBenchmark PRIVATE ../include)
target_link_libraries(biliardoBenchmark PRIVATE ROOT::Core ROOT::Hist benchmark::benchmark)