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

if(CMAKE_CROSSCOMPILING)
  set(IMPORT_EXECUTABLES "IMPORTFILE-NOTFOUND" CACHE FILEPATH "Point it to the export file from a native build")
  include(${IMPORT_EXECUTABLES})
endif(CMAKE_CROSSCOMPILING)

option(USE_QT_GUI "Build Qt GUI" ON)
option(USE_LGTV_GUI "Build LG TV GUI" OFF)
option(USE_RAZE "Build raze emulator" OFF)
option(USE_SDL_AUDIO "Use SDL audio" ON)

include (CheckTypeSize)
check_type_size("unsigned char" SIZEOF_UNSIGNED_CHAR)
check_type_size("unsigned short" SIZEOF_UNSIGNED_SHORT)
check_type_size("unsigned int" SIZEOF_UNSIGNED_INT)
check_type_size("unsigned long" SIZEOF_UNSIGNED_LONG)
check_type_size("unsigned long long" SIZEOF_UNSIGNED_LONG_LONG)
include (CheckIncludeFiles)
check_include_file(inttypes.h HAVE_INTTYPES_H)
check_include_file(memory.h HAVE_MEMORY_H)
check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(strings.h HAVE_STRINGS_H)
check_include_file(string.h HAVE_STRING_H)
check_include_file(sys/stat.h HAVE_SYS_STAT_H)
check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(tcl8.0.h HAVE_TCL8_0_H)
check_include_file(tk8.0.h HAVE_TK8_0_H)
check_include_file(unistd.h HAVE_UNISTD_H)

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


if (USE_QT_GUI)
	find_package(Qt4 4.7 COMPONENTS QtCore QtGui QtMultimedia REQUIRED)
	set(QT_USE_CORE 1)
	set(QT_USE_GUI 1)
	set(QT_USE_MULTIMEDIA 1)
	if (NOT QT_FOUND OR NOT QT_QTCORE_FOUND OR NOT QT_QTGUI_FOUND)
		message(FATAL_ERROR "Package Qt and components QtCore QtGui, required but not found!")
	endif (NOT QT_FOUND OR NOT QT_QTCORE_FOUND OR NOT QT_QTGUI_FOUND)
	include(${QT_USE_FILE})
	include_directories(${QT_INCLUDES})
endif (USE_QT_GUI)

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
if (USE_RAZE)
	add_subdirectory(raze)
else (USE_RAZE)
	add_subdirectory(cmz80)
endif (USE_RAZE)
add_subdirectory(main)

# FIXME: Hardcoded
set(RETSIGTYPE void)

set(VERSION 0.36)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/config.h")
