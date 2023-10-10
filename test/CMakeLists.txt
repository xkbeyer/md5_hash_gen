CMAKE_MINIMUM_REQUIRED(VERSION 3.26)

PROJECT(test)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) #To use with clang-tidy, not supported w/ MSVC generator.

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_TAG        v1.14.0
  GIT_REPOSITORY https://github.com/google/googletest.git
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(SRC md5test.cpp ../src/md5.h ../src/md5.cpp)
add_executable(md5test ${SRC})
target_include_directories(md5test PRIVATE ../src)
target_link_libraries(
  md5test
  GTest::gtest_main
)
target_compile_features(md5test PRIVATE cxx_std_20)
target_compile_definitions(md5test PUBLIC $<$<CONFIG:Debug>:_DEBUG> _CONSOLE)
target_compile_options(md5test PUBLIC 
    $<$<CXX_COMPILER_ID:MSVC>:/Wall /WX /wd5045 /std:c++latest> # binops doesn't work w c++20 it needs c++latest
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
# Remove unwanted project types in MSVC
if(MSVC)
    list(REMOVE_ITEM CMAKE_CONFIGURATION_TYPES MinSizeRel RelWithDebInfo)
endif()
include(GoogleTest)
gtest_discover_tests(md5test)
