cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(async-proc C)

file(GLOB SOURCES
	"src/*.c"
	"src/*.h"
	)
add_executable(${PROJECT_NAME} ${SOURCES})

