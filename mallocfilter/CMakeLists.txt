cmake_minimum_required(VERSION 3.16.3)

project(mallocfreefilter VERSION 1.0.0 DESCRIPTION "malloc free filter library")

add_library(${PROJECT_NAME} SHARED
    src/mallocfreefilter.c
)

add_executable(${PROJECT_NAME}.static 
	src/mallocfreemain.c
)

target_include_directories(${PROJECT_NAME} PRIVATE src)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

# Set the public header property to the one with the actual API.
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER src/mallocfreefilter.h)
