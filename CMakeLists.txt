cmake_minimum_required(VERSION 4.2.3)

# set the project name
project(vortexEngine CXX C)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add the executable
add_subdirectory(vortex)
add_subdirectory(editor)
add_executable(main main.cpp)
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)
target_link_libraries(main PRIVATE glm::glm)
target_link_libraries(main PRIVATE glfw)
target_link_libraries(main PUBLIC vortex_libs)
target_link_libraries(main PUBLIC editor_libs)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
