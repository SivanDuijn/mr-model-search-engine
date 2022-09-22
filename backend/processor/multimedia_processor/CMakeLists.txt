# CMakeList.txt : CMake project for multimedia_processor, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (multimedia_processor "multimedia_processor.cpp" "multimedia_processor.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET multimedia_processor PROPERTY CXX_STANDARD 20)
endif()

# Copy executable to server
add_custom_command(
    TARGET "multimedia_processor"
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    # TODO ask Sivan for CMake variable of top level dir
    ARGS -E copy $<TARGET_FILE:multimedia_processor> ..\\..\\..\\..\\..\\server
)
