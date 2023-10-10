CMAKE_MINIMUM_REQUIRED(VERSION 3.26)

PROJECT(MD5)

enable_testing()

# Setup the cmake directory containing numerous cmake scripts and macros.
set(CMAKE_MODULE_PATH "${MD5_SOURCE_DIR}/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) #To use with clang-tidy, not supported w/ MSVC generator.

set(SRC src/main.cpp src/md5.h src/md5.cpp)

add_executable(md5 ${SRC})
target_include_directories(md5 PUBLIC src)
target_compile_features(md5 PRIVATE cxx_std_20)
target_compile_definitions(md5 PUBLIC $<$<CONFIG:Debug>:_DEBUG> _CONSOLE)
target_compile_options(md5 PUBLIC 
    $<$<CXX_COMPILER_ID:MSVC>:/Wall /WX /wd5045 /std:c++latest> # binops doesn't work w c++20 it needs c++latest
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
# Remove unwanted project types in MSVC
if(MSVC)
    list(REMOVE_ITEM CMAKE_CONFIGURATION_TYPES MinSizeRel RelWithDebInfo)
    set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT md5)
endif()

add_subdirectory(test)