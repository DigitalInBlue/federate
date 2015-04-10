#
# Top level makefile for Federate
#
# John Farrier
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Project Name
PROJECT(Federate)

include(CheckFunctionExists)
include(CheckCXXSourceCompiles)
include(CheckIncludeFile)

if(MSVC) 
  # Force to always compile with warning level 4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]") 
	string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}") 
  else() 
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4") 
  endif() 

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "Compiler Flags for All Builds" FORCE) 
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}" CACHE STRING "Compiler Flags for Debug Builds" FORCE) 
  set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "Compiler Flags for Release Builds" FORCE) 

  add_definitions("/D_CRT_SECURE_NO_WARNINGS /wd4251 /nologo")
endif() 
 
if(BORLAND) 
  set(CMAKE_CXX_FLAGS "" CACHE STRING "Compiler Flags for All Builds" FORCE) 
  set(CMAKE_CXX_FLAGS_DEBUG "" CACHE STRING "Compiler Flags for Debug Builds" FORCE)  
  set(CMAKE_CXX_FLAGS_RELEASE  "" CACHE STRING "Compiler Flags for Release Builds" FORCE) 
endif() 

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	EXEC_PROGRAM(xcrun ARGS -find clang++ OUTPUT_VARIABLE XCODE_CLANG_PATH)
	set (XCODE_CLANG_PATH "${XCODE_CLANG_PATH}" CACHE STRING "Xcode clang path")
	set (CMAKE_CXX_COMPILER "${XCODE_CLANG_PATH}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++") 
	set (CMAKE_COMPILER_IS_GNUCXX 1)
endif()
 
if(CMAKE_COMPILER_IS_GNUCXX) 
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -std=gnu++11 -Wall" CACHE STRING "Compiler Flags for All Builds" FORCE) 
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pedantic" CACHE STRING "Compiler Flags for Debug Builds" FORCE) 
  set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "Compiler Flags for Release Builds" FORCE) 
endif() 

set(CMAKE_DEBUG_POSTFIX          "d" CACHE STRING "add a postfix, usually d on windows")
set(CMAKE_RELEASE_POSTFIX        ""  CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_RELWITHDEBINFO_POSTFIX ""  CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_MINSIZEREL_POSTFIX     ""  CACHE STRING "add a postfix, usually empty on windows")

# --------------------------------------------------------------------------- #
# Header-Only Library
# --------------------------------------------------------------------------- #
set(FEDERATE_SOURCE_DIR ./)
set(HEADER_PATH ${FEDERATE_SOURCE_DIR}/include)

set(TARGET_H
	include/Federate/Federate.h
	)

set(TARGET_SRC
	)

include_directories(${HEADER_PATH})
add_custom_Target(Federate SOURCES ./include/Federate/Federate.h)

# --------------------------------------------------------------------------- #
# GTest Unit Tests
# --------------------------------------------------------------------------- #

option(FUNCTIONFEDERATION_GTEST "Set to ON to enable Google tests FunctionFederation." OFF)

if(FUNCTIONFEDERATION_GTEST)
	set(GTEST_DIRECTORY test/gtest-1.7.0)
	set(GTEST_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${GTEST_DIRECTORY}/include)
	set(GTEST_LIBRARY gtest)
	set(GTEST_MAIN_LIBRARY gtest_main)
	include_directories(${FEDERATE_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/${GTEST_DIRECTORY}/include ${CMAKE_CURRENT_SOURCE_DIR}/${GTEST_DIRECTORY}/src)
	ADD_SUBDIRECTORY(${GTEST_DIRECTORY})

	add_executable(FederateTest
		test/test.cpp
		)

	# VS2012 doesn't support true variadic templates
	if(MSVC) 
		add_definitions( /D _VARIADIC_MAX=10 )
	endif()

	target_link_libraries(FederateTest
		${GTEST_LIBRARY} 
		${GTEST_MAIN_LIBRARY} 
		)
endif()
