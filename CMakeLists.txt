cmake_minimum_required(VERSION 3.0.2)
project(winter VERSION 0.1.0 LANGUAGES C)

set(CMAKE_BUILD_TYPE DEBUG)

set(WINTER_SOURCES
	src/wlex.c
	src/wtable.c
	src/wparse.c
	src/test.c
)
include_directories(./include)

if (NOT MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic")
endif()

add_executable(${PROJECT_NAME} ${WINTER_SOURCES})
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)
