# [[Root]]
cmake_minimum_required(VERSION 3.5)

project(Eightest)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# [[Options]]
option(EIGHTEST_BUILD_SHARED_LIBS "Build shared libraies by Default" ON)


# [[Defaults]]
if(LINUX)
    set(EIGHTEST_RPATH "$ORIGIN")
elseif(APPLE)
    set(EIGHTEST_RPATH "@loader_path/../Frameworks")
endif()


# [[Binaries]]
if(EIGHTEST_BUILD_SHARED_LIBS)
    set(PROJECT_LIBS_TYPE SHARED)
else()
    set(PROJECT_LIBS_TYPE STATIC)
endif()

file(GLOB_RECURSE PROJECT_SOURCES_FILES "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/include/*.hpp")
add_library(Eightest ${PROJECT_LIBS_TYPE} ${PROJECT_SOURCES_FILES})

target_include_directories(Eightest PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")

if(EIGHTEST_BUILD_SHARED_LIBS)
    set_target_properties(Eightest PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()


# [[Launcher]]
if(EIGHTEST_CORE_RUN)
    add_executable(EightestLauncher "${CMAKE_CURRENT_LIST_DIR}/test/Launcher.cpp")

    target_link_libraries(EightestLauncher PUBLIC Eightest)
    if(LINUX)
        target_link_libraries(EightestLauncher PUBLIC dl)
    endif()

    set_target_properties(EightestLauncher PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE INSTALL_RPATH "${EIGHTEST_RPATH}" MACOSX_BUNDLE $<BOOL:${APPLE}>)
    target_compile_definitions(EightestLauncher PUBLIC "EIGHTEST_CORE_RUN=${EIGHTEST_CORE_RUN}")
endif()
