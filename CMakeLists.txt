cmake_minimum_required(VERSION 3.14)

project(GameLive LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} main.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)

find_package(SFML COMPONENTS system window graphics audio CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-system sfml-network sfml-graphics sfml-window sfml-audio)

file(GLOB DLL_FILES_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/SFML/bin/*.dll")
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${DLL_FILES_RELEASE}
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)

# Copy shader file after build
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/main_shader.fs"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)

