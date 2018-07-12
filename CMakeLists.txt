cmake_minimum_required(VERSION 3.10)
project(mp4parser)

set(CMAKE_CXX_STANDARD 14)

file(GLOB ISOBMFF_SRC
        "include/ISOBMFF/*.hpp"
        "include/ISOBMFF/*.h"
        "include/*.h"
        "include/ISOBMFF.hpp"
        "include/XS/PIMPL/*.hpp"
        "src/*.cpp"
        )

include_directories(include)

add_library(isobmff ${ISOBMFF_SRC})
add_executable(mp4StreamDump tools/mp4StreamDump.cpp)
add_executable(aacExtractor tools/aacExtractor.cpp)
target_link_libraries(mp4StreamDump isobmff boost_system)
target_link_libraries(aacExtractor isobmff boost_system)

file(COPY ${PROJECT_SOURCE_DIR}/tests/output.m4s DESTINATION ${CMAKE_BINARY_DIR}/tests/)