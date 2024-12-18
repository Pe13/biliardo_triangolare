# abilita il supporto per i test
include(CTest)

# test biliardo
add_executable(TPool_test biliardoTest.cpp)

target_include_directories(TPool_test PRIVATE ../../include)
target_link_libraries(TPool_test PRIVATE TPool ROOT::Core ROOT::Hist TBB::tbb)

add_test(NAME TPool_test COMMAND TPool_test)