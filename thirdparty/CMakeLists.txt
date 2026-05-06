include(FetchContent)

FetchContent_Declare(
    CPM
    URL https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/CPM.cmake
    DOWNLOAD_NO_EXTRACT TRUE
)

FetchContent_MakeAvailable(CPM)
include(${cpm_SOURCE_DIR}/CPM.cmake)

# OpenGL
find_package(OpenGL REQUIRED)

# glad
set(GLAD_DIR ${CMAKE_CURRENT_LIST_DIR}/glad)
add_library(glad STATIC ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PUBLIC ${GLAD_DIR}/include)

# stb_image
set(STB_IMAGE_DIR ${CMAKE_CURRENT_LIST_DIR}/stb_image)
add_library(stb_image STATIC ${STB_IMAGE_DIR}/stb_image.cpp)
target_include_directories(stb_image PUBLIC ${STB_IMAGE_DIR})

#stb_vorbis
set(STB_VORBIS_DIR ${CMAKE_CURRENT_LIST_DIR}/stb_vorbis)
add_library(stb_vorbis STATIC ${STB_VORBIS_DIR}/stb_vorbis.c)
target_include_directories(stb_vorbis PUBLIC ${STB_VORBIS_DIR})


# other deps
CPMAddPackage("gh:assimp/assimp@5.4.3")
CPMAddPackage("gh:glfw/glfw#3.3.8")
CPMAddPackage("gh:g-truc/glm#1.0.1")
CPMAddPackage("gh:kcat/openal-soft#1.23.1")
CPMAddPackage("gh:nlohmann/json@3.11.3")

set(FT_DISABLE_ZLIB TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2 TRUE CACHE BOOL "" FORCE)

CPMAddPackage(
    NAME freetype
    GITHUB_REPOSITORY freetype/freetype
    GIT_TAG VER-2-13-2
)

if (TARGET freetype)
    add_library(Freetype::Freetype ALIAS freetype)
endif()

set_target_properties(glad
                      stb_image
					  stb_vorbis
                      assimp 
                      glfw 
                      glm 
                      freetype
                      PROPERTIES FOLDER "thirdparty")

if (TARGET zlibstatic)
    set_target_properties(zlibstatic PROPERTIES FOLDER "thirdparty")
endif()