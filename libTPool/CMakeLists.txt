add_library(TPool Biliardo.cpp)

target_include_directories(TPool PUBLIC include)
target_link_libraries(TPool PRIVATE ROOT::Core ROOT::Hist TBB::tbb)

if (BUILD_BENCHMARK)
  add_subdirectory(benchmarks)
endif ()

if (BUILD_TESTING)
  add_subdirectory(tests)
endif ()