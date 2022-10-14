# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required(VERSION 3.8)

project("multimedia_processor" VERSION 0.1.0)

set(ASSETS_PATH ${PROJECT_SOURCE_DIR})
configure_file(variables.h.in ${PROJECT_SOURCE_DIR}/variables.h)

# compile PMP library
set(PMP_BUILD_EXAMPLES OFF)
set(PMP_BUILD_TESTS OFF)
set(PMP_BUILD_DOCS OFF)
set(PMP_BUILD_VIS OFF)
add_subdirectory(submodules/pmp-library)

# Include sub-projects.
add_subdirectory("multimedia_processor")
