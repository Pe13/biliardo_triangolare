cmake_minimum_required(VERSION 3.22)
project(biliardo_triangolare)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# richiedi l'uso di C++17, senza estensioni non-standard offerte dal compilatore usato
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

# abilita warning
string(APPEND CMAKE_CXX_FLAGS
       " -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion"
       " -Wshadow -Wimplicit-fallthrough -Wextra-semi -Wold-style-cast")


# rimuovi le ottimizzazioni se si deve profilare l'eseguibile
if (PROFILE OR VALGRIND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
endif ()

# abilita l'address sanitizer e l'undefined-behaviour sanitizer in debug mode quando non si usa valgrind
if (NOT VALGRIND)
  string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fsanitize=address,undefined -fno-omit-frame-pointer")
  string(APPEND CMAKE_EXE_LINKER_FLAGS_DEBUG " -fsanitize=address,undefined -fno-omit-frame-pointer")
endif ()

find_package(ROOT 6.26 COMPONENTS CONFIG REQUIRED)
find_package(SFML 2.5 COMPONENTS graphics REQUIRED)
find_package(TGUI 1.0 REQUIRED)
find_package(Boost 1.35 COMPONENTS container REQUIRED)
find_package(TBB REQUIRED)

# compila il backend
add_subdirectory(TPool)

# crea l'app per il frontend
add_executable(biliardo_triangolare
               main.cpp App.cpp
               Designer.cpp
               Gui.cpp)

add_dependencies(biliardo_triangolare tpool)
target_include_directories(biliardo_triangolare PRIVATE include)
target_link_libraries(biliardo_triangolare PRIVATE tpool ROOT::Core ROOT::Hist ROOT::Gpad sfml-graphics TGUI::TGUI TBB::tbb)

# copia le texture nella directory di build
add_custom_command(
        TARGET biliardo_triangolare POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/resources/Black.png
        ${CMAKE_CURRENT_BINARY_DIR}/Black.png &&
        ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/resources/Black.txt
        ${CMAKE_CURRENT_BINARY_DIR}/Black.txt
        )

# se il testing e' abilitato...
# per disabilitare il testing, passare -DBUILD_TESTING=OFF a cmake durante la fase di configurazione
if (BUILD_TESTING)
  add_subdirectory(tests)
endif ()