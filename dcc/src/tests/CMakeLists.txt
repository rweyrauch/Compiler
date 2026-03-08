cmake_minimum_required(VERSION 2.8)

find_package(CppUnit QUIET REQUIRED)
find_package(Popt QUIET REQUIRED)
  
# Specify include directories
set(IR_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/ir)

include_directories(
    ${IR_INCLUDE_DIR}
    ${CPPUNIT_INCLUDE_DIRS}
    )
  
# For compilation ...
# Specify target & source files to compile it from
add_executable(
    testDccIr
    main.cpp
    SymbolTableTests.cpp
    )
 
# For linking ...
# Specify target & libraries to link it with
target_link_libraries(
    testDccIr
    IrAst
    libcppunit.a
    popt
    )

# Specify library paths
link_directories(
    )
