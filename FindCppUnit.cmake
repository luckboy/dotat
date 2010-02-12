# find include directory
find_path(CPPUNIT_INCLUDE_DIR cppunit/TestCase.h
	PATHS "${PROJECT_SOURCE_DIR}/../include"
		"${PROJECT_SOURCE_DIR}/../../include"
		"${PROJECT_SOURCE_DIR}/../../../include")

# find library
find_library(CPPUNIT_LIBRARY cppunit
	PATHS "${PROJECT_SOURCE_DIR}/../lib"
		"${PROJECT_SOURCE_DIR}/../../lib"
		"${PROJECT_SOURCE_DIR}/../../../lib")

# found CppUnit / not found CppUnit
set(CPPUNIT_FOUND OFF)
if(CPPUNIT_INCLUDE_DIR AND CPPUNIT_LIBRARY)
  set(CPPUNIT_FOUND ON)
  set(CPPUNIT_LIBRARIES ${CPPUNIT_LIBRARY} ${CMAKE_DL_LIBS})
endif()
