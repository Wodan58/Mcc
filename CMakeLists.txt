#
#   module  : CMakeLists.txt
#   version : 1.17
#   date    : 10/30/23
#
cmake_minimum_required(VERSION 3.0)
# replace 1.0 by I within percents before posting
project(MCC VERSION 1.17)
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
  set(CMAKE_BUILD_TYPE "Release")
endif()
option(RUN_TESTS "Run standard tests" OFF)
find_package(BISON)
find_package(FLEX)
BISON_TARGET(MyParser ${CMAKE_SOURCE_DIR}/pars.y ${CMAKE_SOURCE_DIR}/pars.c
	     DEFINES_FILE ${CMAKE_SOURCE_DIR}/pars.h)
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  FLEX_TARGET(MyScanner ${CMAKE_SOURCE_DIR}/lexr.l ${CMAKE_SOURCE_DIR}/lexr.c
              COMPILE_FLAGS "--wincompat")
  add_definitions(-DGC_NOT_DLL -D_CRT_SECURE_NO_WARNINGS)
else()
  FLEX_TARGET(MyScanner ${CMAKE_SOURCE_DIR}/lexr.l ${CMAKE_SOURCE_DIR}/lexr.c)
endif()
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)
add_definitions(-DVERSION="${CMAKE_BUILD_TYPE} ${CMAKE_PROJECT_VERSION}")
add_executable(mcc mcc.c ${FLEX_MyScanner_OUTPUTS} ${BISON_MyParser_OUTPUTS}
               name.c ylex.c)
add_executable(mcd mcd.c name.c)
add_dependencies(mcd mcc)
add_executable(mci mci.c name.c)
add_dependencies(mci mcc)
add_executable(mcp mcp.c name.c)
add_dependencies(mcp mcc)
add_executable(mca mca.c)
add_dependencies(mca mcc)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  add_subdirectory(bdwgc)
  include_directories(bdwgc/include)
  target_link_libraries(mcc gc-lib)
else()
  if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 --coverage -fprofile-arcs -ftest-coverage") # debug, no optimization
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage") # enabling coverage
  endif()
  target_link_libraries(mcc m gc)
endif()

if(RUN_TESTS)
add_custom_target(input.out ALL
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		  COMMAND mcc input.txt >input.out
		  DEPENDS input.txt)
add_custom_target(input.bin ALL
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		  COMMAND mcd input.bin <input.out
		  DEPENDS input.out)
add_custom_target(input.s ALL
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		  COMMAND mca input.bin >input.s
		  DEPENDS input.bin)
add_custom_target(input.o ALL
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		  COMMAND nasm -oinput.o -fwin64 input.s
		  DEPENDS input.s)
add_custom_target(input ALL
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		  COMMAND gcc -oinput input.o
		  DEPENDS input.o)
endif()
