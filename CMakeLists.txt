cmake_minimum_required(VERSION 3.0.0)
project(scratchfps VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(rasterizer main.cpp)

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(scratchfps
    PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
