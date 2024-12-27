cmake_minimum_required(VERSION 3.30)

project(Test)

set(TARGET_NAME asiotest)

# This is the name of the driver according to SteamVR
set(DRIVER_NAME "driver_${TARGET_NAME}")

file(GLOB_RECURSE CLIENT_SRC ${CMAKE_SOURCE_DIR}/src/*.cpp
     ${CMAKE_SOURCE_DIR}/src/*.hpp)

file(GLOB_RECURSE SERVER_SRC ${CMAKE_SOURCE_DIR}/src/*.cpp
     ${CMAKE_SOURCE_DIR}/src/*.hpp)

file(GLOB_RECURSE DRIVER_SRC
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.hpp
    ${CMAKE_SOURCE_DIR}/src/*.h
)

list(FILTER CLIENT_SRC EXCLUDE REGEX ${CMAKE_SOURCE_DIR}/src/server)
list(FILTER CLIENT_SRC EXCLUDE REGEX ${CMAKE_SOURCE_DIR}/src/driver)

list(FILTER DRIVER_SRC EXCLUDE REGEX ${CMAKE_SOURCE_DIR}/src/client)
list(FILTER DRIVER_SRC EXCLUDE REGEX ${CMAKE_SOURCE_DIR}/src/server)

set(THREADS_PREFER_PTHREAD_FLAD ON)
set(BUILD_SHARED ON)

find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)
find_package(PNG REQUIRED)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED system)

set(OPENVR_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/openvr)

# If not set, determine the platform architecture
if(NOT PLATFORM)
  if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(PLATFORM 64)
  else()
    set(PLATFORM 32)
  endif()
endif()
message(STATUS "Compilation set for ${PLATFORM}bits architectures.")

# OpenVR compatibility checking
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_definitions(-DLINUX -DPOSIX)
  set(ARCH_TARGET linux64)

  if(${PLATFORM} MATCHES 32)
    message(WARNING "OpenVR x86 binaries not provided on GNU/Linux.")
  endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_MACOSX_RPATH 0)
  add_definitions(-DOSX -DPOSIX)
  set(ARCH_TARGET osx32)

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  add_definitions(-D_WIN32)
  set(ARCH_TARGET win${PLATFORM})

  # Binaries path for thirdparties are not generics so we try to guess their suffixes.
  set(WINDOWS_PATH_SUFFIXES win${PLATFORM} Win${PLATFORM} x${PLATFORM})
endif()

find_library(OPENVR_LIBRARIES
  NAMES
    openvr_api
  PATHS
    ${OPENVR_LIB_DIR}/bin
    ${OPENVR_LIB_DIR}/lib
  PATH_SUFFIXES
    osx${PLATFORM}
    linux${PLATFORM}
    win${PLATFORM}
  NO_DEFAULT_PATH
  NO_CMAKE_FIND_ROOT_PATH
)

set(OPENVR_INCLUDE_DIR ${OPENVR_LIB_DIR}/headers)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(USED_LIBS
	Threads::Threads
	OpenGL::OpenGL
	OpenGL::GL
	OpenGL::GLX
	${X11_LIBRARIES}
        PNG::PNG
	${Boost_LIBRARIES}
)

# ---------------------------- client --------------------------------

add_executable(client ${CLIENT_SRC})
target_include_directories(client PUBLIC
	"${CMAKE_CURRENT_LIST_DIR}/olcPixelGameEngine/"
	"${CMAKE_CURRENT_LIST_DIR}/olcPixelGameEngine/utilities/"
	"${CMAKE_CURRENT_LIST_DIR}/olcPixelGameEngine/extensions/"
	"${CMAKE_SOURCE_DIR}/src/common"
	${PNG_INCLUDE_DIR}
)
target_link_libraries(client PUBLIC ${USED_LIBS})
# target_compile_options(client PRIVATE "-Werror" "-Wall" "-Wextra")

# ---------------------------- driver --------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)

add_library(${DRIVER_NAME} SHARED ${DRIVER_SRC})

# This is so we can build directly to "<binary_dir>/<target_name>/<platform>/<arch>/<driver_name>.<dll/so>"
set_target_properties(${DRIVER_NAME} PROPERTIES
      ARCHIVE_OUTPUT_DIRECTORY
      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}/bin/${ARCH_TARGET}
      LIBRARY_OUTPUT_DIRECTORY
      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}/bin/${ARCH_TARGET}
      RUNTIME_OUTPUT_DIRECTORY
      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}/bin/${ARCH_TARGET}
)
set_target_properties(${DRIVER_NAME} PROPERTIES PREFIX "")

target_include_directories(${DRIVER_NAME} PUBLIC
	"${CMAKE_CURRENT_LIST_DIR}/olcPixelGameEngine/"
	"${CMAKE_CURRENT_LIST_DIR}/olcPixelGameEngine/utilities/"
	"${CMAKE_CURRENT_LIST_DIR}/olcPixelGameEngine/extensions/"
	"${CMAKE_SOURCE_DIR}/src/common"
	${PNG_INCLUDE_DIR}
	${OPENVR_INCLUDE_DIR}
)
target_link_libraries(${DRIVER_NAME} PUBLIC
	${OPENVR_LIBRARIES}
	${USED_LIBS}
)
# target_compile_options(server PRIVATE "-Werror" "-Wall" "-Wextra")

# Copy driver assets to output folder
add_custom_command(
        TARGET ${DRIVER_NAME}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_NAME}
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}
)

