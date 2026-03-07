cmake_minimum_required(VERSION 3.21 FATAL_ERROR)
project(OpenGLGP VERSION 2.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE debug)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(global_settings)

# ---- Dependencies ----
add_subdirectory(thirdparty)

# ---- Main project's files ----
add_subdirectory(src)
