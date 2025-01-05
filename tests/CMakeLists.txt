# abilita il supporto per i test
include(CTest)

# test gui
add_executable(Gui_test guiTest.cpp)

target_include_directories(Gui_test PRIVATE ../include)
target_link_libraries(Gui_test PRIVATE TGUI::TGUI ROOT::Hist)

add_test(NAME Gui_test COMMAND Gui_test)

# test app
add_executable(App_test
               ../App.cpp
               ../Gui.cpp
               ../Designer.cpp
               appConstructorTests.cpp
               appBiliardoModificationsTests.cpp
               appLaunchesTests.cpp)

add_dependencies(App_test TPool)
target_include_directories(App_test PRIVATE ../include)
target_link_libraries(App_test PRIVATE TPool ROOT::Hist ROOT::Gpad sfml-graphics TGUI::TGUI)

add_test(NAME App_test COMMAND App_test)