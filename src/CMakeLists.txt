cmake_minimum_required(VERSION 3.17.3)
project(swamp_gameplay_lib C)

set(CMAKE_C_STANDARD 11)
add_compile_definitions(_POSIX_C_SOURCE=200112L TORNADO_OS_LINUX CONFIGURATION_DEBUG TYRAN_CONFIGURATION_DEBUG)

include_directories(include)

include_directories(../deps/piot/basal-c/src/include)
include_directories(../deps/piot/clog/src/include)
include_directories(../deps/piot/tiny-libc/src/include)
include_directories(../deps/piot/imprint/src/include)
include_directories(../deps/piot/breathe/src/include)
include_directories(../deps/piot/burst/src/include)
include_directories(../deps/piot/latch/src/include)
include_directories(../deps/piot/monotonic-time-c/src/include)
include_directories(../deps/swamp/runtime-c/src/include)
include_directories(../deps/swamp/typeinfo-c/src/include)
include_directories(../deps/swamp/dump-c/src/include)
include_directories(../deps/piot/swamp-script-scope/src/include)
include_directories(../deps/swamp/unpack-c/src/include)

file(GLOB_RECURSE deps_src FOLLOW_SYMLINKS
        "../deps/*/src/*.c"
        )

# Remove iOS and SDL (including Webassembly)
list(FILTER deps_src EXCLUDE REGEX ".*sdl_main.c" )
list(FILTER deps_src EXCLUDE REGEX ".*webassembly/main.c" )
list(FILTER deps_src EXCLUDE REGEX ".*src/examples/main.c" )
list(FILTER deps_src EXCLUDE REGEX ".*src/example/main.c" )
list(FILTER deps_src EXCLUDE REGEX ".*src/example/app.c" )
list(FILTER deps_src EXCLUDE REGEX ".*src/platform/ios/.*" )
list(FILTER deps_src EXCLUDE REGEX ".*src/platform/posix/.*" )
list(FILTER deps_src EXCLUDE REGEX ".*src/platform/pulseaudio/.*" )
list(FILTER deps_src EXCLUDE REGEX ".*src/platform/webassembly/.*" )
list(FILTER deps_src EXCLUDE REGEX ".*src/platform/sdl/.*" )


file(GLOB_RECURSE lib_src FOLLOW_SYMLINKS
        "lib/*.c"
        )

add_library(swamp_gameplay_lib
        ${deps_src}
        ${lib_src}
        )

target_link_libraries(swamp_gameplay_lib m)
target_link_libraries(swamp_gameplay_lib -fsanitize=address)
