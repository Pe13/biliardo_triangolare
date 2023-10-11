cmake_minimum_required(VERSION 3.22)
project(biliardo_triangolare)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# richiedi l'uso di C++17, senza estensioni non-standard offerte dal compilatore usato
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

# abilita warning
string(APPEND CMAKE_CXX_FLAGS " -Wall -Wextra")

# rimuove le ottimizzazioni se si deve profilare l'eseguibile
if (PROFILE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
endif ()

# abilita l'address sanitizer e l'undefined-behaviour sanitizer in debug mode
string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fsanitize=address,undefined -fno-omit-frame-pointer")
string(APPEND CMAKE_EXE_LINKER_FLAGS_DEBUG " -fsanitize=address,undefined -fno-omit-frame-pointer")

find_package(ROOT 6.26 COMPONENTS CONFIG REQUIRED)
find_package(SFML 2.5 COMPONENTS graphics REQUIRED)
find_package(TGUI 1.0 REQUIRED)
find_package(Boost 1.35 COMPONENTS container REQUIRED)

add_executable(biliardo_triangolare
        main.cpp App.cpp
        Biliardo.cpp Designer.cpp
        BiliardoFunctions.cpp
        Gui.cpp)

target_include_directories(biliardo_triangolare PRIVATE include)
target_link_libraries(biliardo_triangolare PRIVATE ROOT::Core ROOT::Hist ROOT::Physics ROOT::Gpad sfml-graphics TGUI::TGUI)

# se il testing e' abilitato...
#   per disabilitare il testing, passare -DBUILD_TESTING=OFF a cmake durante la fase di configurazione
if (BUILD_TESTING)
    add_subdirectory(tests)
endif ()