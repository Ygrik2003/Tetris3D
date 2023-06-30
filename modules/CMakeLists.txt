cmake_minimum_required(VERSION 3.22)

project(modules)

if(EXISTS "${CMAKE_SOURCE_DIR}/modules/SDL3/CMakeLists.txt")
    message(STATUS "SDL3 doesnt found")
    message(STATUS "Compiling...")

    set(SDL_STATIC ON CACHE INTERNAL "" FORCE)
    set(SDL_SHARED ON CACHE INTERNAL "" FORCE)
    set(SDL_STATIC_PIC ON CACHE INTERNAL "" FORCE)
    set(SDL_TEST OFF CACHE INTERNAL "" FORCE)
    set(SDL_TESTS OFF CACHE INTERNAL "" FORCE)
    set(SDL_TESTS_LINK_SHARED OFF CACHE INTERNAL "" FORCE)
    add_subdirectory(${CMAKE_SOURCE_DIR}/modules/SDL3
                     ${CMAKE_BINARY_DIR}/lib/SDL3)
    message(STATUS "Compiling SDL3 is succesuful.")
elseif(NOT SDL3_FOUND)
    message(
        FATAL_ERROR
            "Cant find SDL source, please try execute:\n  git submodule init\n  git submodule update"
        )
endif()

set(SDL3_DIR ${CMAKE_BINARY_DIR}/lib/SDL3 CACHE INTERNAL "SDL3_DIR")

message(
    STATUS
        "SDL version find: ${SDL3_VERSION_MAJOR}.${SDL3_VERSION_MINOR}.${SDL3_VERSION_PATCH}"
    )

if(EXISTS "${CMAKE_SOURCE_DIR}/modules/Assimp/CMakeLists.txt")

    set(ENABLE_BOOST_WORKAROUND OFF CACHE INTERNAL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE INTERNAL "" FORCE)
    set(ASSIMP_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
    set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE INTERNAL "" FORCE)
    set(ASSIMP_INSTALL OFF CACHE INTERNAL "" FORCE)

    if(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
        option(ASSIMP_ANDROID_JNIIOSYSTEM ON)
    endif()

    add_subdirectory(${CMAKE_SOURCE_DIR}/modules/Assimp/
                     ${CMAKE_BINARY_DIR}/lib/Assimp)
    message(STATUS "ASSIMP compiled!")
else()
    message(
        FATAL_ERROR
            "Cant find Assimp source, please try execute:\ngit submodule init\ngit submodule update"
        )
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/modules/imgui/")

    file(
        GLOB
        SOURCES
        imgui/*.h
        imgui/*.cpp)

    add_library(imgui STATIC ${SOURCES})
    target_include_directories(imgui
                               PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/modules/imgui)
    set_property(TARGET imgui PROPERTY POSITION_INDEPENDENT_CODE ON)

    message(STATUS "imgui compiled!")
else()
    message(
        FATAL_ERROR
            "Cant find imgui source, please try execute:\ngit submodule init\ngit submodule update"
        )
endif()
