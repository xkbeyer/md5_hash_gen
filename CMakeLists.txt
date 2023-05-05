CMAKE_MINIMUM_REQUIRED(VERSION 3.23)

PROJECT(MD5)

# Setup the cmake directory containing numerous cmake scripts and macros.
set(CMAKE_MODULE_PATH "${MD5_SOURCE_DIR}/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) #To use with clang-tidy, not supported w/ MSVC generator.

set(SRC main.cpp md5.h md5.cpp)

add_executable(md5 ${SRC})

target_compile_features(md5 PRIVATE cxx_std_20)
target_compile_definitions(md5 PRIVATE $<$<CONFIG:Debug>:_DEBUG> _CONSOLE)
# Remove unwanted project types in MSVC
if(MSVC)
  list(REMOVE_ITEM CMAKE_CONFIGURATION_TYPES MinSizeRel RelWithDebInfo)
  set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT md5)
endif()
