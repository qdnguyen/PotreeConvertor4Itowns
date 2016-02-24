project(PotreeConverter)

set(RAPIDJSON_INCLUDE_DIR lib/rapidjson/include)

file(GLOB_RECURSE files src/*.cpp include/*.h include/*.hpp )
file(GLOB_RECURSE lib_rapidjson_files ${RAPIDJSON_INCLUDE_DIR}/* )

include_directories(include ${LASZIP_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
include_directories(include ${RAPIDJSON_INCLUDE_DIR})

SOURCE_GROUP("Header Files\\io" FILES 
	include/BINPointReader.hpp 
	include/BINPointWriter.hpp
	include/LASPointReader.h
	include/LASPointWriter.hpp
	include/PlyPointReader.h
	include/PointReader.h
	include/PointWriter.hpp
	include/PTXPointReader.h
	include/XYZPointReader.hpp
    include/Projector.hpp
)

SOURCE_GROUP("Source Files\\io" FILES 
	src/BINPointReader.cpp
	src/LASPointReader.cpp
	src/LASPointWriter.cpp
	src/PTXPointReader.cpp
    src/Projector.cpp
)

SOURCE_GROUP("lib\\rapidjson" FILES ${lib_rapidjson_files})

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
find_package(proj4 REQUIRED)
IF(PROJ4_FOUND)
    include_directories(${PROJ4_INCLUDE_DIR})
	message(PROJ4=${PROJ4_INCLUDE_DIR} ${PROJ4_LIBRARY})
ENDIF()


add_executable(PotreeConverter ${files} ${lib_rapidjson_files})

target_link_libraries(PotreeConverter ${LASZIP_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_REGEX_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${PROJ4_LIBRARY})

install(TARGETS PotreeConverter RUNTIME DESTINATION bin/ )