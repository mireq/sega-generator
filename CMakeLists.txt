project(Generator C CXX)

cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0003 NEW)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include_directories(
	"${PROJECT_BINARY_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_CURRENT_BINARY_DIR}"
	${QT_INCLUDES}
	${QJSON_INCLUDE_DIRS}
)

option(ENABLE_QT "Build Qt GUI" ON)
option(ENABLE_RAZE "Build raze emulator" OFF)
option(USE_SDL_AUDIO "Use SDL audio" ON)

include (CheckTypeSize)
check_type_size("unsigned char" SIZEOF_UNSIGNED_CHAR)
check_type_size("unsigned short" SIZEOF_UNSIGNED_SHORT)
check_type_size("unsigned int" SIZEOF_UNSIGNED_INT)
check_type_size("unsigned long" SIZEOF_UNSIGNED_LONG)
check_type_size("unsigned long long" SIZEOF_UNSIGNED_LONG_LONG)

include (TargetArch)
target_architecture(CMAKE_ARCH)
if (${CMAKE_ARCH} STREQUAL "arm")
	set(PROCESSOR_ARM 1)
endif (${CMAKE_ARCH} STREQUAL "arm")
if (${CMAKE_ARCH} STREQUAL "i386")
	set(PROCESSOR_i386 1)
endif (${CMAKE_ARCH} STREQUAL "i386")
if (${CMAKE_ARCH} STREQUAL "x86_64")
	set(PROCESSOR_x86_64 1)
endif (${CMAKE_ARCH} STREQUAL "x86_64")
if (${CMAKE_ARCH} STREQUAL "sparc")
	set(PROCESSOR_SPARC 1)
endif (${CMAKE_ARCH} STREQUAL "sparc")


if (ENABLE_QT)
	find_package(Qt4 4.7 COMPONENTS QtCore QtGui QtMultimedia REQUIRED)
	set(QT_USE_CORE 1)
	set(QT_USE_GUI 1)
	set(QT_USE_MULTIMEDIA 1)
	if (NOT QT_FOUND OR NOT QT_QTCORE_FOUND OR NOT QT_QTGUI_FOUND)
		message(FATAL_ERROR "Package Qt and components QtCore QtGui, required but not found!")
	endif (NOT QT_FOUND OR NOT QT_QTCORE_FOUND OR NOT QT_QTGUI_FOUND)
	include(${QT_USE_FILE})
	include_directories(${QT_INCLUDES})
endif (ENABLE_QT)

if (USE_SDL_AUDIO)
	find_package(SDL REQUIRED)
	if (NOT SDL_FOUND)
		message(FATAL_ERROR "Package SDL, required but not found!")
	endif (NOT SDL_FOUND)
else (USE_SDL_AUDIO)
endif (USE_SDL_AUDIO)

add_subdirectory(cpu68k)
add_subdirectory(sn76496)
add_subdirectory(ym2612)
if (ENABLE_RAZE)
	add_subdirectory(raze)
else (ENABLE_RAZE)
	add_subdirectory(cmz80)
endif (ENABLE_RAZE)
add_subdirectory(main)

# FIXME: Hardcoded
set(RETSIGTYPE void)

set(VERSION 0.36)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/config.h")
