cmake_minimum_required(VERSION 3.0.2)
project(winter VERSION 0.0.1 LANGUAGES C)

set(CMAKE_BUILD_TYPE DEBUG)

set(WINTER_SOURCES
	src/lexer.c
	src/object.c
	src/parser.c
	src/table.c
	src/winter.c
	src/test.c
	src/wstring.c
)

if (NOT MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -pedantic")
	# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s -Os -march=native")
endif()

add_executable(${PROJECT_NAME} ${WINTER_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ./include)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)
