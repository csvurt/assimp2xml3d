cmake_minimum_required (VERSION 2.6)
project (assimp2xml3d)

########## Version number
set(ASSIMP2XML3D_VERSION_MAJOR, 0)
set(ASSIMP2XML3D_VERSION_MINOR, 1)

########## Set cmake variables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_INSTALL_BINDIR "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")

########## Set Assimp variables
set ( ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL
	"Disable building the Assimp toolkit (eg. Assimp model viewer)"
)

########## Include Assimp
add_subdirectory (assimp)
include_directories("assimp/include")
set (ASSIMP_LIBS ${ASSIMP_LIBS} assimp)

########## Include TinyXML-2
add_subdirectory (tinyxml2)
include_directories("tinyxml2/")
set (TINYXML_LIBS ${TINYXML_LIBS} tinyxml2)

########## Link executable
add_executable(assimp2xml3d src/main.cpp src/xml3d_exporter.cpp src/xml3d_exporter.h)
target_link_libraries (assimp2xml3d  ${ASSIMP_LIBS} ${TINYXML_LIBS})

########## Install executable to bin dir
INSTALL( TARGETS assimp2xml3d
	 LIBRARY DESTINATION ${ASSIMP_LIB_INSTALL_DIR}
	 RUNTIME DESTINATION ${ASSIMP_BIN_INSTALL_DIR})