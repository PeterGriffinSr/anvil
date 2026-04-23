cmake_minimum_required(VERSION 3.20)
project(
    anvil
    VERSION 0.1.0
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

file(GLOB_RECURSE core_srcs
    "${PROJECT_SOURCE_DIR}/irbuilder/**.hpp"
    "${PROJECT_SOURCE_DIR}/ir/**.hpp"
    "${PROJECT_SOURCE_DIR}/libanvil/**.cpp")

add_library(libanvil STATIC ${core_srcs})

target_include_directories(libanvil
    PUBLIC
        ${PROJECT_SOURCE_DIR}/irbuilder
        ${PROJECT_SOURCE_DIR}/ir
        ${PROJECT_SOURCE_DIR}
)

set_target_properties(libanvil PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(libanvil PROPERTIES VERSION ${PROJECT_VERSION})

add_library(libanvil_c STATIC ${PROJECT_SOURCE_DIR}/capi/anvil_c.cpp)

target_include_directories(libanvil_c
    PUBLIC
        ${PROJECT_SOURCE_DIR}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/ir
        ${PROJECT_SOURCE_DIR}/irbuilder
)

target_link_libraries(libanvil_c PUBLIC libanvil)
set_target_properties(libanvil_c PROPERTIES VERSION ${PROJECT_VERSION})

set(WARN_FLAGS
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /EHsc>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Wno-sign-compare>
)
target_compile_options(libanvil   PRIVATE ${WARN_FLAGS})
target_compile_options(libanvil_c PRIVATE ${WARN_FLAGS})

foreach(example hello_world call complex complex_two nested)
    add_executable(${example} ${PROJECT_SOURCE_DIR}/examples/${example}.cpp)
    target_link_libraries(${example} PRIVATE libanvil)
endforeach()

add_executable(hello_world_c ${PROJECT_SOURCE_DIR}/examples/hello_world_c.c)
target_link_libraries(hello_world_c PRIVATE libanvil_c)
