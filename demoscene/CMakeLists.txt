PROJECT(main)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

IF(MSVC)
add_definitions(-DWIN32)
add_definitions(-D__STDC_LIMIT_MACROS)
ELSEIF(APPLE)
SET(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
SET(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

SET(CMAKE_XCODE_ATTRIBUTE_CLANG_C_LANGUAGE_STANDARD "c11")
SET(CMAKE_XCODE_ATTRIBUTE_CLANG_C_LIBRARY "libc")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++ -g -Wall")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -g -Wall")
SET(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "Build architectures for OSX" FORCE)
ELSE()
SET(CMAKE_CXX_FLAGS_RELEASE "-std=c++11 -DNDEBUG -W -Wall -Wno-sign-compare -O2 -s -pipe -mmmx -msse -msse2 -msse3")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-std=c++11 -g -DNDEBUG -W -Wall -Wno-sign-compare -O2 -s -pipe -mmmx -msse -msse2 -msse3")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -std=c++11")
ENDIF()

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR} /usr/local/include /opt/local/include)
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR} /usr/local/lib /opt/local/lib)

###########

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# ===
SET(Fire_SRC
  ../platform_info/platform_info.h
  ../time/tc_timer.h
  ../random/tc_random_funcs.h
  ../sdl/sdl.h
  main_fire.cpp
)

ADD_EXECUTABLE(main_fire ${Fire_SRC})
target_link_libraries(main_fire ${SDL2_LIBRARIES})

# ===
SET(DoomFire_SRC
../platform_info/platform_info.h
../time/tc_timer.h
../random/tc_random_funcs.h
../sdl/sdl.h
main_doom_fire.cpp
)

ADD_EXECUTABLE(main_doom_fire ${DoomFire_SRC})
target_link_libraries(main_doom_fire ${SDL2_LIBRARIES})

# ===
SET(Water_SRC
  ../platform_info/platform_info.h
  ../time/tc_timer.h
  ../random/tc_random_funcs.h
  ../sdl/sdl.h
  main_water.cpp
)

ADD_EXECUTABLE(main_water ${Water_SRC})
target_link_libraries(main_water ${SDL2_LIBRARIES})
