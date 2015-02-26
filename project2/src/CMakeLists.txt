project(Decaf)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11")

include_directories(ir . ${CMAKE_BUILD_DIR})

set(BisonOutput ${CMAKE_SOURCE_DIR}/parse.cc)
set(FlexOutput ${CMAKE_SOURCE_DIR}/lex.cc)

add_custom_command(
	OUTPUT ${BisonOutput} Parserbase.h
	COMMAND bisonc++ --construction decaf.y
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
add_custom_command(
	OUTPUT ${FlexOutput} Scannerbase.h
	COMMAND flexc++ --construction decaf.l
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_executable(
	dcc
	main.cpp
	ScannerStreambuf.cpp
	${FlexOutput}
	${BisonOutput}
)

target_link_libraries(
	dcc
	IrAst
	popt
)

install(
    TARGETS dcc
    DESTINATION ${CMAKE_SOURCE_DIR}/..
)

add_subdirectory("ir")
