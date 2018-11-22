cmake_minimum_required(VERSION 3.0.2)
project(Winter VERSION 0.1.0 LANGUAGES C)

set(WINTER_SOURCES
	src/wlex.c
)
include_directories(./include)

add_executable(${PROJECT_NAME} ${WINTER_SOURCES})
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)
