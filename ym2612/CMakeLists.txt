include_directories(
	"${CMAKE_SOURCE_DIR}/hdr"
	"${CMAKE_BINARY_DIR}/hdr"
	"${CMAKE_BINARY_DIR}"
)

add_library(ym6212 STATIC fm.c)
