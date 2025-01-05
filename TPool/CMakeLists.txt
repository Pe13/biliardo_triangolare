add_library(tpool Pool.cpp)

target_include_directories(tpool PUBLIC include)
target_link_libraries(tpool PRIVATE ROOT::Core ROOT::Hist TBB::tbb)

if (BUILD_BENCHMARK)
  add_subdirectory(benchmarks)
endif ()

if (BUILD_TESTING)
  add_subdirectory(tests)
endif ()