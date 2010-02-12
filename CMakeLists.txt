cmake_minimum_required(VERSION 2.6)

project(dotat)

if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-Wall -std=c++98)
endif()

add_library(dotat STATIC dotat.cpp)
add_executable(dotatrun dotatrun.cpp)
target_link_libraries(dotatrun dotat)

if(BUILD_TESTING)
  enable_testing()
  include("${PROJECT_SOURCE_DIR}/FindCppUnit.cmake")
  if(CPPUNIT_FOUND)
    add_executable(testdotat testdotat.cpp)
    target_link_libraries(testdotat dotat ${CPPUNIT_LIBRARIES})
    add_test(testdotat "${PROJECT_BINARY_DIR}/testdotat")
  endif()
endif()
