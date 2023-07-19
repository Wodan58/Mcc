#
#   module  : CMakeLists.txt
#   version : 1.1
#   date    : 07/19/23
#
cmake_minimum_required(VERSION 3.0)
project(Mcc VERSION 1.0)
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
  set(CMAKE_BUILD_TYPE "Release")
endif()
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
               name.c)
add_executable(mcd mcd.c name.c)
add_executable(mci mci.c name.c)
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_link_libraries(mcc gc-lib)
  include_directories(bdwgc/include)
  add_subdirectory(bdwgc)
else()
  if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 --coverage -fprofile-arcs -ftest-coverage") # debug, no optimization
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage") # enabling coverage
  endif()
  target_link_libraries(mcc m gc)
endif()