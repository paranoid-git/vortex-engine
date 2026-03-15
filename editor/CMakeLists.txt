file(GLOB_RECURSE VORTEX_SOURCES
    "src/*.cpp"
    "src/*/*.cpp"
    "src/*/*/*.cpp"
    "src/*.c"
    "src/*/*.c"
    "src/*/*/*.c"
    "include/*.h"
    "include/*/*.h"
    "include/*/*/*.h"
)



add_library(editor_libs STATIC ${VORTEX_SOURCES})


target_include_directories(editor_libs

    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include

)
target_include_directories(editor_libs
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
)
# ImGui
#
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/vendor/imgui)
add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)
target_link_libraries(imgui PUBLIC glfw)

# link it to your main target
target_link_libraries(editor_libs imgui)
target_link_libraries(editor_libs vortex_libs)
