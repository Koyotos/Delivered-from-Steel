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
set(glad_SOURCE_DIR ${GLAD_DIR}/include CACHE INTERNAL "")
add_library(glad STATIC ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PRIVATE ${GLAD_DIR}/include)

# stb_image
set(STB_IMAGE_DIR ${CMAKE_CURRENT_LIST_DIR}/stb_image)
set(stb_image_SOURCE_DIR ${STB_IMAGE_DIR} CACHE INTERNAL "")
add_library(stb_image STATIC ${STB_IMAGE_DIR}/stb_image.cpp)
target_include_directories(stb_image PRIVATE ${STB_IMAGE_DIR})

# other
CPMAddPackage("gh:assimp/assimp@5.4.3")
CPMAddPackage("gh:glfw/glfw#3.3.8")
CPMAddPackage("gh:g-truc/glm#1.0.1")
CPMAddPackage("gh:ocornut/imgui@1.88")
CPMAddPackage("gh:kcat/openal-soft#1.23.1")
CPMAddPackage("gh:nlohmann/json@3.11.3")

set(imgui_SOURCE_DIR ${imgui_SOURCE_DIR} CACHE INTERNAL "")
add_library(imgui STATIC ${imgui_SOURCE_DIR}/imgui.cpp
					     ${imgui_SOURCE_DIR}/imgui_demo.cpp
					     ${imgui_SOURCE_DIR}/imgui_draw.cpp
						 ${imgui_SOURCE_DIR}/imgui_tables.cpp
					     ${imgui_SOURCE_DIR}/imgui_widgets.cpp)
						 
set_target_properties(glad
                      stb_image 
                      assimp 
                      glfw 
                      glm 
                      imgui PROPERTIES FOLDER "thirdparty")

if (TARGET zlibstatic)
    set_target_properties(zlibstatic PROPERTIES FOLDER "thirdparty")
endif()
