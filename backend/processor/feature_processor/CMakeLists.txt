# CMakeList.txt : CMake project for multimedia_processor, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.8)

# add include directories
include_directories(${PROJECT_SOURCE_DIR}/submodules/pmp-library/src)
include_directories(${PROJECT_SOURCE_DIR}/submodules/pmp-library/external/eigen)

file(GLOB_RECURSE sources *.cpp *.h)

# Add source to this project's executable.
add_executable(feature_processor ${sources})
target_link_libraries(feature_processor pmp)

if(CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET feature_processor PROPERTY CXX_STANDARD 20)
endif()
