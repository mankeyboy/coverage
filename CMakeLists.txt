cmake_minimum_required(VERSION 2.8)
project(swarm_robotics)

find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIR} )
include_directories( include )


set( SHARED_SRC src/VoronoiDiagramGenerator.cpp include/VoronoiDiagramGenerator.h)

set( VORONOI_MAIN src/voronoi_main.cpp ${SHARED_SRC})

add_executable( voronoi_main ${VORONOI_MAIN})

target_link_libraries(voronoi_main ${OpenCV_LIBS})
