find_package(benchmark REQUIRED)

add_executable(biliardoBenchmark
        fillingBenchmarks.cpp
        LaunchingBenchmarks.cpp
        ../Biliardo.cpp)

target_include_directories(biliardoBenchmark PRIVATE ../include)
target_link_libraries(biliardoBenchmark PRIVATE ROOT::Core ROOT::Hist benchmark::benchmark)