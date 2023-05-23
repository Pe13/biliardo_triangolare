cmake_minimum_required(VERSION 3.22)
project(biliardo_triangolare)

# abilita il supporto per i test, tra cui l'opzione BUILD_TESTING usata sotto
include(CTest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# richiedi l'uso di C++17, senza estensioni non-standard offerte dal compilatore usato
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

# abilita warning
string(APPEND CMAKE_CXX_FLAGS " -Wall -Wextra")

# abilita l'address sanitizer e l'undefined-behaviour sanitizer in debug mode
string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fsanitize=address,undefined -fno-omit-frame-pointer")
string(APPEND CMAKE_EXE_LINKER_FLAGS_DEBUG " -fsanitize=address,undefined -fno-omit-frame-pointer")

find_package(ROOT 6.20 COMPONENTS CONFIG REQUIRED)
find_package(SFML 2.5 COMPONENTS graphics REQUIRED)

add_executable(biliardo_triangolare main.cpp App.cpp include/App.hpp)

target_include_directories(biliardo_triangolare PRIVATE include)
target_link_libraries(biliardo_triangolare PRIVATE ROOT::Core ROOT::Hist ROOT::Physics sfml-graphics)