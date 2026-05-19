cmake_minimum_required(VERSION 3.21 FATAL_ERROR)
project(FishEngine VERSION 2.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../game/arch)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../game/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../game)


list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(global_settings)

set(CMAKE_CXX_FLAGS_RELEASE
    "-O3 -march=native -flto -fprofile-generate -DNDEBUG"
)

set(CMAKE_C_FLAGS_RELEASE
    "-O3 -march=native -flto -fprofile-generate -DNDEBUG"
)

# ---- Dependencies ----
add_subdirectory(thirdparty)

# ---- Main project's files ----
add_subdirectory(src)
