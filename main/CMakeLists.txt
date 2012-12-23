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

set (GENERATOR_COMMON_SOURCES
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
	uiplot.c
	gensoundp-unix.c
	initcart.c
	patch.c
	dib.c
	avi.c
	diss68k.c
)

if (USE_RAZE)
	set(Z80_LIB ${CMAKE_BINARY_DIR}/raze/libraze.a)
else (USE_RAZE)
	set(Z80_LIB ${CMAKE_BINARY_DIR}/cmz80/libcmz80.a)
endif (USE_RAZE)


if (USE_SDL_AUDIO)
	set(SOUND_LIBS ${SDL_LIBRARY})
	set(SOUND_INCLUDES ${SDL_INCLUDE_DIR})
else (USE_SDL_AUDIO)
	set(SOUND_LIBS)
	set(SOUND_INCLUDES)
endif (USE_SDL_AUDIO)

if (USE_QT_GUI)
	set (GENERATOR_QT_SOURCES
		${GENERATOR_COMMON_SOURCES}
		ui-qt.cpp
		qt/ConfigDialog.cpp
		qt/ConfigDialog.h
		qt/ConfigPanel.cpp
		qt/ConfigPanel.h
		qt/KeySequenceEdit.cpp
		qt/KeySequenceEdit.h
		qt/Icon.cpp
		qt/Icon.h
		qt/Panels.cpp
		qt/Panels.h
		qt/QtGeneratorWindow.cpp
		qt/QtGeneratorWindow.h
		qt/QtXvControlsWidget.cpp
		qt/QtXvControlsWidget.h
		qt/QtXvWidget.cpp
		qt/QtXvWidget.h
	)
	qt4_wrap_cpp(MOC_SOURCES
		qt/QtGeneratorWindow.h
		qt/QtXvControlsWidget.h
		qt/QtXvWidget.h
		qt/ConfigDialog.h
		qt/ConfigPanel.h
		qt/KeySequenceEdit.h
		qt/Panels.h
	)
	qt4_add_resources(GENERATOR_QRC_SRCS qt/resources.qrc)

	include_directories(${QT_INCLUDES} ${SOUND_INCLUDES})
	add_executable(generator-qt ${GENERATOR_QT_SOURCES} ${MOC_SOURCES} ${GENERATOR_QRC_SRCS})
	target_link_libraries(generator-qt
		${Z80_LIB}
		${SOUND_LIBS}
		${CMAKE_BINARY_DIR}/cpu68k/lib68k.3.a
		${CMAKE_BINARY_DIR}/sn76496/libsn76496.a
		${CMAKE_BINARY_DIR}/ym2612/libym6212.a
		${QT_LIBRARIES}
		X11
		Xv
		m
	)
endif (USE_QT_GUI)

if (USE_LGTV_GUI)
	set (GENERATOR_LG_SOURCES
		${GENERATOR_COMMON_SOURCES}
		ui-lgtv.c
	)
	add_executable(generator-lg ${GENERATOR_LG_SOURCES})
	target_link_libraries(generator-lg
		${Z80_LIB}
		${SOUND_LIBS}
		${CMAKE_BINARY_DIR}/cpu68k/lib68k.3.a
		${CMAKE_BINARY_DIR}/sn76496/libsn76496.a
		${CMAKE_BINARY_DIR}/ym2612/libym6212.a
		m
	)
endif (USE_LGTV_GUI)
