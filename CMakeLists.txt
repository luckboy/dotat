cmake_minimum_required(VERSION 2.6)

project(dotat)

if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-Wall -std=c++98)
endif()

if(BUILD_SHARED_LIBS)
  if(MSVC)
    add_definitions(/Ddotat_SHARED)
  else()
    add_definitions(-Ddotat_SHARED)
  endif()
endif()

add_library(dotat dotat.cpp)
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

install_files(/include .hpp "${PROJECT_SOURCE_DIR}/dotat.hpp")
install_targets(/lib dotat)
install_targets(/bin dotatrun)
install_files(/share/doc/dotat .txt "${PROJECT_SOURCE_DIR}/LICENSE.txt" "${PROJECT_SOURCE_DIR}/CZYTAJTO.txt")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "My esoteric programming language")
set(CPACK_PACKAGE_VENDOR "Lukasz Szpakowski")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/CZYTAJTO.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "1")
set(CPACK_SOURCE_IGNORE_FILES
  "/\\\\.git/"
  "/\\\\.gitignore"
  ".*~"
  ".*\\\\.a"
  ".*\\\\.lib"
  ".*\\\\.dll"
  ".*\\\\.so"
  ".*\\\\.exe"
  ".*\\\\.exp"
  ".*\\\\.res"
  "/dotatrun$"
  "/testdotat$"
  "/CMakeFiles/"
  "/Testing/"
  "/_CPack_Packages/"
  "/CMakeCache\\\\.txt"
  "/CPackConfig\\\\.cmake"
  "/CPackSourceConfig\\\\.cmake"
  "/CTestTestfile\\\\.cmake"
  "/Makefile"
  "/cmake_install\\\\.cmake"
  "/install_manifest\\\\.txt"
  "/dotat-.*"
  ".*\\\\.manifest"
  ".*\\\\.resource\\\\.txt")

include(CPack)
