enable_language(ASM_NASM)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/raze.inc
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/raze.inc ${CMAKE_CURRENT_BINARY_DIR}/raze.inc
	MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/raze.inc
)
add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/raze2.asm
	COMMAND nasm -e ${CMAKE_CURRENT_SOURCE_DIR}/raze.asm -o ${CMAKE_CURRENT_BINARY_DIR}/raze2.asm
	MAIN_DEPENDENCY  ${CMAKE_CURRENT_BINARY_DIR}/raze.inc
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/raze.asm
)

#add_custom_target(
#	raze.o ALL
#	COMMAND nasm -f elf ${CMAKE_CURRENT_BINARY_DIR}/raze2.asm -o ${CMAKE_CURRENT_BINARY_DIR}/raze.o -p${CMAKE_CURRENT_SOURCE_DIR}/raze.reg
#	DEPENDS raze2.asm ${CMAKE_CURRENT_SOURCE_DIR}/raze.reg
#)
#set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/raze2.asm PROPERTIES COMPILE_FLAGS " -p${CMAKE_CURRENT_SOURCE_DIR}/raze.reg ")
set(CMAKE_ASM_NASM_FLAGS " -p${CMAKE_CURRENT_SOURCE_DIR}/raze.reg ")

add_library(raze STATIC ${CMAKE_CURRENT_BINARY_DIR}/raze2.asm)
