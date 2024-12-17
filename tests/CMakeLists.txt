# abilita il supporto per i test
include(CTest)

# test gui
add_executable(test_gui guiTest.cpp)

target_include_directories(test_gui PRIVATE ../include)
target_link_libraries(test_gui PRIVATE TGUI::TGUI ROOT::Hist)

add_test(NAME test_gui COMMAND test_gui)
