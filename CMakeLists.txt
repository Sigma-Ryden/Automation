# [[Root]]
cmake_minimum_required(VERSION 3.5)

project(Eightest)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# [[Options]]
option(EIGHTEST_BUILD_SHARED_LIBS "Build shared libraies by Default" ON)


# [[Binaries]]
if(EIGHTEST_BUILD_SHARED_LIBS)
    set(PROJECT_LIBS_TYPE SHARED)
else()
    set(PROJECT_LIBS_TYPE STATIC)
endif()

file(GLOB_RECURSE PROJECT_SOURCES_FILES "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/include/*.hpp")
add_library("${PROJECT_NAME}" ${PROJECT_LIBS_TYPE} ${PROJECT_SOURCES_FILES})

target_include_directories("${PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")

if(EIGHTEST_BUILD_SHARED_LIBS)
    set_target_properties("${PROJECT_NAME}" PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()
