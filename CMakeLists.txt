#*****************************************************************************#
#    Copyright (C) 2022 Julian Xhokaxhiu                                      #
#                                                                             #
#    This file is part of MMNx                                                #
#                                                                             #
#    MMNx is free software: you can redistribute it and/or modify             #
#    it under the terms of the GNU General Public License as published by     #
#    the Free Software Foundation, either version 3 of the License            #
#                                                                             #
#    MMNx is distributed in the hope that it will be useful,                  #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of           #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            #
#    GNU General Public License for more details.                             #
#*****************************************************************************#

cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)

if(NOT DEFINED CMAKE_BUILD_TYPE)
  message(FATAL_ERROR "CMAKE_BUILD_TYPE must be set to continue building with cmake. \nExample: Add -DCMAKE_BUILD_TYPE=Release to your cmake command line.")
endif()
if (NOT DEFINED _EXE_VERSION OR NOT _EXE_VERSION)
  message(FATAL_ERROR "_EXE_VERSION must be set to continue building with cmake. \nExample: Add -D_EXE_VERSION=devel to your cmake command line.")
endif ()

set(VCPKG_DISABLE_COMPILER_TRACKING 1)
set(VCPKG_INSTALL_OPTIONS "--clean-after-build" "--x-use-aria2")
set(VCPKG_TARGET_TRIPLET x86-windows-static)

set(CMAKE_SHARED_LINKER_FLAGS
  "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT /NODEFAULTLIB:MSVCRTD /DEBUG:FULL /FORCE:MULTIPLE /IGNORE:4006,4075,4099,4217"
)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
  set(MMNX_SCITERJS_DLL_PATH "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/bin/sciter.dll")
else()
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:LIBCMTD")
  set(MMNX_SCITERJS_DLL_PATH "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/sciter.dll")
endif()

project(MMNx)

find_package(SCITERJS REQUIRED)

set(RELEASE_NAME "MMNx")

if(_EXE_VERSION STREQUAL "devel" OR _EXE_VERSION MATCHES "-")
  set(_EXE_RCVERSION "0,0,0,0")
  set(_EXE_RCSTRVERSION "0.0.0.0")
else()
  string(REPLACE "." "," _EXE_RCVERSION ${_EXE_VERSION})
  set(_EXE_RCSTRVERSION ${_EXE_VERSION})
endif()

# Include all the source code files
file(GLOB_RECURSE source_files "${CMAKE_SOURCE_DIR}/src/*.cpp")

configure_file(misc/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc @ONLY)

add_executable(
  ${RELEASE_NAME}
  WIN32
  ${source_files}
  ${CMAKE_CURRENT_BINARY_DIR}/version.rc
)
target_include_directories(
  ${RELEASE_NAME}
  PRIVATE ${CMAKE_SOURCE_DIR}/src
  PRIVATE ${SCITERJS_INCLUDE_DIRS}
)
target_link_libraries(
  ${RELEASE_NAME}
  ${SCITERJS_LIBRARIES}
)
target_compile_options(
  ${RELEASE_NAME}
  PRIVATE /DVERSION="${_EXE_VERSION}"
  PRIVATE /D_CRT_SECURE_NO_WARNINGS
  PRIVATE /DNOMINMAX
  PRIVATE /Zc:strictStrings-
  PRIVATE /Qpar
)
target_compile_features(
  ${RELEASE_NAME}
  PRIVATE cxx_std_20
)
target_link_options(
  ${RELEASE_NAME}
  PRIVATE /PDBALTPATH:${RELEASE_NAME}.pdb
)

# RESOURCE COMPILATION
add_custom_command(
  TARGET ${RELEASE_NAME}
  PRE_BUILD
  # packfolder.exe
  COMMAND
    ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/tools/sciterjs/packfolder ${CMAKE_SOURCE_DIR}/ui ${CMAKE_SOURCE_DIR}/src/resources.cpp -v "resources"
)

# INSTALL
add_custom_command(
  TARGET ${RELEASE_NAME}
  POST_BUILD
  # ensure bin directory exists
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin
  # License
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/COPYING.TXT
          ${CMAKE_BINARY_DIR}/bin
  # .exe
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${RELEASE_NAME}.exe
          ${CMAKE_BINARY_DIR}/bin
  # .pdb
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${RELEASE_NAME}.pdb
          ${CMAKE_BINARY_DIR}/bin
  # sciter.dll
  COMMAND ${CMAKE_COMMAND} -E copy
          ${MMNX_SCITERJS_DLL_PATH}
          ${CMAKE_BINARY_DIR}/bin
)
