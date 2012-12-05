include_directories(
	"${CMAKE_SOURCE_DIR}/hdr"
	"${CMAKE_BINARY_DIR}/hdr"
	"${CMAKE_BINARY_DIR}"
)

add_library(sn76496 sn76496.c sn76496.h)
