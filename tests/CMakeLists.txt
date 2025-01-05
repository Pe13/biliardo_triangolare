# abilita il supporto per i test
include(CTest)

# test gui
add_executable(gui_test guiTest.cpp)

target_include_directories(gui_test PRIVATE ../include)
target_link_libraries(gui_test PRIVATE TGUI::TGUI ROOT::Hist)

add_test(NAME gui_test COMMAND gui_test)

# test app
add_executable(app_test
               ../App.cpp
               ../Gui.cpp
               ../Designer.cpp
               appConstructorTests.cpp
               appBiliardoModificationsTests.cpp
               appLaunchesTests.cpp)

add_dependencies(app_test tpool)
target_include_directories(app_test PRIVATE ../include)
target_link_libraries(app_test PRIVATE tpool ROOT::Hist ROOT::Gpad sfml-graphics TGUI::TGUI)

add_test(NAME app_test COMMAND app_test)