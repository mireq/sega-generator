include_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${CMAKE_SOURCE_DIR}/cpu68k"
	"${CMAKE_BINARY_DIR}/cpu68k"
	"${CMAKE_SOURCE_DIR}/ym2612"
	"${CMAKE_SOURCE_DIR}/raze"
	"${CMAKE_SOURCE_DIR}/cmz80"
	"${CMAKE_SOURCE_DIR}/sn76496"
	"${CMAKE_SOURCE_DIR}/hdr"
)

set (GENERATOR_QT_SOURCES
	cpu68k.c
	mem68k.c
	reg68k.c
	event.c
	generator.c
	state.c
	cpuz80.c
	vdp.c
	gensound.c
	memz80.c
	ui-qt.cpp
	uiplot.c
	gensoundp-unix.c
	initcart.c
	patch.c
	dib.c
	avi.c
	diss68k.c
)

if (ENABLE_RAZE)
	set(Z80_LIB ${CMAKE_BINARY_DIR}/raze/libraze.a)
else (ENABLE_RAZE)
	set(Z80_LIB ${CMAKE_BINARY_DIR}/cmz80/libcmz80.a)
endif (ENABLE_RAZE)


if (USE_SDL_AUDIO)
	set(SOUND_LIBS ${SDL_LIBRARY})
	set(SOUND_INCLUDES ${SDL_INCLUDE_DIR})
else (USE_SDL_AUDIO)
	set(SOUND_LIBS)
	set(SOUND_INCLUDES)
endif (USE_SDL_AUDIO)

if (ENABLE_QT)
	include_directories(${QT_INCLUDES} ${SOUND_INCLUDES})
	add_executable(generator-qt ${GENERATOR_QT_SOURCES})
	target_link_libraries(generator-qt
		${Z80_LIB}
		${SOUND_LIBS}
		${CMAKE_BINARY_DIR}/cpu68k/lib68k.3.a
		${CMAKE_BINARY_DIR}/sn76496/libsn76496.a
		${CMAKE_BINARY_DIR}/ym2612/libym6212.a
		${QT_LIBRARIES}
	)
endif (ENABLE_QT)
