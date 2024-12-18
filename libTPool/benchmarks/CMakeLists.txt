find_package(benchmark REQUIRED)

add_executable(biliardoBenchmark
        fillingBenchmarks.cpp
        LaunchingBenchmarks.cpp
        )

#target_include_directories(biliardoBenchmark PRIVATE ../include)
target_link_libraries(biliardoBenchmark PRIVATE TPool ROOT::Core ROOT::Hist benchmark::benchmark benchmark::benchmark_main)