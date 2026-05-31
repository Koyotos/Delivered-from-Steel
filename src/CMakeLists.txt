# Add source files
file(GLOB_RECURSE SOURCE_FILES 
	 *.c
	 *.cpp)
	
# Add header files
file(GLOB_RECURSE HEADER_FILES 
	 *.h
	 *.hpp)
	 
# Search for the assets files
file(GLOB_RECURSE ASSETS_FILES 
	 ${CMAKE_SOURCE_DIR}/res/*.*)

set_source_files_properties(${ASSETS_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})						   
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES})
source_group(TREE ${CMAKE_SOURCE_DIR}         FILES ${ASSETS_FILES})

# Define the executable
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${ASSETS_FILES})

target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)
target_compile_definitions(${PROJECT_NAME} PRIVATE LIBRARY_SUFFIX="")

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
												  ${glad_SOURCE_DIR}
												  ${stb_image_SOURCE_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${freetype_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} glad)
target_link_libraries(${PROJECT_NAME} stb_image)
target_link_libraries(${PROJECT_NAME} stb_vorbis)
target_link_libraries(${PROJECT_NAME} assimp)
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} glm::glm)
target_link_libraries(${PROJECT_NAME} OpenAL::OpenAL)
target_link_libraries(${PROJECT_NAME} nlohmann_json)
target_link_libraries(${PROJECT_NAME} Freetype::Freetype)
if(WIN32)
    target_link_libraries(${PROJECT_NAME} xinput)
endif()


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
				   COMMAND ${CMAKE_COMMAND} -E create_symlink 
				   ${CMAKE_SOURCE_DIR}/res 
				   ${CMAKE_CURRENT_BINARY_DIR}/res)

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PUBLIC NOMINMAX)
endif()
