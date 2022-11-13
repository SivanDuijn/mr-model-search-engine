# CMakeList.txt : CMake project for multimedia_processor, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.8)

# Nuke all warnings out of orbit
# Only necessary because CMake won't listen to anything else >:(
add_definitions(-w)

# Add include directories
include_directories(${PROJECT_SOURCE_DIR}/submodules/pmp-library/src)
include_directories(${PROJECT_SOURCE_DIR}/submodules/pmp-library/external/eigen)

# Add source to this project's executable.
file(GLOB_RECURSE sources *.cpp *.h "../lib/libgdiam/gdiam.cpp")
add_executable(multimedia_processor ${sources})
target_link_libraries(multimedia_processor pmp)

if(CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET multimedia_processor PROPERTY CXX_STANDARD 20)
endif()

# Copy executable to server
if (UNIX)
  set(EXEC_FILE "multimedia_processor")
else()
  set(EXEC_FILE "multimedia_processor.exe")
endif (UNIX)

add_custom_command(
  TARGET "multimedia_processor"
  POST_BUILD
  COMMAND ${CMAKE_COMMAND}
  ARGS -E copy $<TARGET_FILE:multimedia_processor> ${CMAKE_SOURCE_DIR}/../server/processors/${EXEC_FILE}
)
