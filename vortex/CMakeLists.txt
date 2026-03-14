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



add_library(vortex_libs STATIC ${VORTEX_SOURCES})


target_include_directories(vortex_libs 
  
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
