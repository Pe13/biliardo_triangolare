# abilita il supporto per i test
include(CTest)

# test biliardo
add_executable(tpool_test
               constructorTests.cpp
               bouncingAlgorithmTests.cpp
               parametersSettersTests.cpp
               typeSetterTests.cpp
               launchFunctionsTests.cpp)

target_include_directories(tpool_test PRIVATE ../../include)
target_link_libraries(tpool_test PRIVATE tpool ROOT::Core ROOT::Hist TBB::tbb)

add_test(NAME tpool_test COMMAND tpool_test)