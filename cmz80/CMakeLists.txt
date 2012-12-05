include_directories(
	"${CMAKE_SOURCE_DIR}/hdr"
	"${CMAKE_BINARY_DIR}/hdr"
	"${CMAKE_BINARY_DIR}"
)

set(CMZ_SOURCES
	z80.c
)

set(CMZ_HEADERS
	cmz80.h
	z80stb.h
	z80stbd.h
)

add_library(cmz80 STATIC ${CMZ_SOURCES} ${CMZ_HEADERS})
