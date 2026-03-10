cmake_minimum_required(VERSION 3.5)
project(RhombusRampage)

if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
    add_compile_options("-sUSE_FREETYPE=1")
endif()

add_subdirectory(external/SDL EXCLUDE_FROM_ALL)

set(SDL3TTF_VENDORED OFF CACHE BOOL "" FORCE)
add_subdirectory(external/SDL_ttf EXCLUDE_FROM_ALL)

# Create your game executable target as usual
add_executable(RhombusRampage src/main.cpp src/sprite.cpp)

target_include_directories(RhombusRampage PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
# Link to the actual SDL3 library.
target_link_libraries(RhombusRampage PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)

if(EMSCRIPTEN)
    target_link_options(RhombusRampage PRIVATE
        "-sUSE_SDL=0"
        "-sUSE_FREETYPE=1"
        "-sALLOW_MEMORY_GROWTH=1"
        "--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets"
    )
endif()
