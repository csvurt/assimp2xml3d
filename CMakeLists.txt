cmake_minimum_required (VERSION 2.6)
project (assimp2xml3d)

set(ASSIMP2XML3D_VERSION_MAJOR, 0)
set(ASSIMP2XML3D_VERSION_MINOR, 1)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")

set ( ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL
	"Disable building the Assimp toolkit (eg. Assimp model viewer)"
)

add_subdirectory (assimp)
include_directories("assimp/include")
set (ASSIMP_LIBS ${ASSIMP_LIBS} assimp)

add_executable(assimp2xml3d src/main.cpp)
target_link_libraries (assimp2xml3d  ${ASSIMP_LIBS})


INSTALL( TARGETS assimp2xml3d
	 LIBRARY DESTINATION ${ASSIMP_LIB_INSTALL_DIR}
	 RUNTIME DESTINATION ${ASSIMP_BIN_INSTALL_DIR})
