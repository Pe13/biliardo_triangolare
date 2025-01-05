find_package(benchmark REQUIRED)

add_executable(pool_benchmark
        fillingBenchmarks.cpp
        LaunchingBenchmarks.cpp
        )

target_link_libraries(pool_benchmark PRIVATE tpool ROOT::Core ROOT::Hist benchmark::benchmark benchmark::benchmark_main)