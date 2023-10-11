cmake_minimum_required(VERSION 3.22)
project(tests)

# abilita il supporto per i test
include(CTest)

# test biliardo
add_executable(test_biliardo biliardoTest.cpp ../Biliardo.cpp ../BiliardoFunctions.cpp)

target_include_directories(test_biliardo PRIVATE ../include)
target_link_libraries(test_biliardo PRIVATE ROOT::Core ROOT::Hist ROOT::Physics sfml-graphics)

add_test(NAME test_biliardo COMMAND test_biliardo)

# test gui
add_executable(test_gui guiTest.cpp)

target_include_directories(test_gui PRIVATE ../include)
target_link_libraries(test_gui PRIVATE TGUI::TGUI ROOT::Hist)

add_test(NAME test_gui COMMAND test_gui)
