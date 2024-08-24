# Following SDL2 CMakeLists.txt: https://github.com/libsdl-org/SDL/blob/288aea3b40faf882f26411e6a3fe06329bba2c05/CMakeLists.txt
if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR})
  message(FATAL_ERROR "Prevented in-tree build. Please create a build directory outside of the source code and run `cmake ..` (path to project root) from there.")
endif()

cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(CGameSandbox VERSION 0.1.0 LANGUAGES C CXX)
set(EXECUTABLE_NAME "game" CACHE STRING "EXECUTABLE_NAME")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)


set(PROJECT_ROOT_DIR  ${PROJECT_SOURCE_DIR}        CACHE STRING "PROJECT_ROOT_DIR")
set(BUILD_ROOT_DIR    ${CMAKE_CURRENT_BINARY_DIR}  CACHE STRING "BUILD_ROOT_DIR")
set(DIST_DIR          ${BUILD_ROOT_DIR}/dist       CACHE STRING "DIST_DIR")
set(FONT_RELATIVE_PATH "include/adobe-fonts/source-code-pro/TTF/SourceCodePro-Regular.ttf")

message("CMAKE_CURRENT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
message("CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")

set(CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR}/include/)
file(GLOB_RECURSE CPP_SOURCE ${PROJECT_ROOT_DIR}/src/*.cpp)


option(USE_EMSCRIPTEN "Use emscripten to compile for WASM. Docs: https://emscripten.org/docs" OFF)

if (NOT USE_EMSCRIPTEN)
  # This will currently only work on the MacOS:
  message("Not compiling for WASM")

  # Find SDL and SDL_ttf
  find_package(SDL2 CONFIG REQUIRED)
  find_package(SDL2_ttf CONFIG REQUIRED)
  get_target_property(SDL2_ttf_INCLUDE_DIRS SDL2_ttf::SDL2_ttf INTERFACE_INCLUDE_DIRECTORIES)
  
  message("SDL2_INCLUDE_DIRS: ${SDL2_INCLUDE_DIRS}")
  message("SDL2_ttf_INCLUDE_DIRS: ${SDL2_ttf_INCLUDE_DIRS}")

  include_directories(CGameSandbox ${SDL2_INCLUDE_DIRS} ${SDL2_ttf_INCLUDE_DIRS})

  add_executable(${EXECUTABLE_NAME} ${CPP_SOURCE})
  target_link_libraries(${EXECUTABLE_NAME} PRIVATE SDL2::SDL2 SDL2_ttf::SDL2_ttf)
  set_target_properties(${EXECUTABLE_NAME} PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${DIST_DIR}"
  )

  # Copy font file.  It would be nice to be able to copy the entire directoy
  configure_file(
    "${PROJECT_ROOT_DIR}/${FONT_RELATIVE_PATH}" 
    "${DIST_DIR}/${FONT_RELATIVE_PATH}" 
    COPYONLY
  )
  

else()
  # There must be a cleaner way to do this, but I can't figure out how
  # to get `--use-port=sdl2` to be included in the compile commands
  # without using a custom command.

  message("Compiling for WASM")
  set(EMSCRIPTEN_ROOT_DIR ${PROJECT_ROOT_DIR}/include/emsdk/upstream/emscripten 
    CACHE STRING "EMSCRIPTEN_ROOT_DIR"
  )
  set(EMCC ${EMSCRIPTEN_ROOT_DIR}/emcc CACHE STRING "EMCC")
  set(CMAKE_TOOLCHAIN_FILE 
      ${EMSCRIPTEN_ROOT_DIR}/emscripten/cmake/Modules/Platform/Emscripten.cmake  
      CACHE STRING "CMAKE_TOOLCHAIN_FILE"
  )
  message("EMSCRIPTEN_ROOT_DIR: ${EMSCRIPTEN_ROOT_DIR}")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "CMAKE_BUILD_TYPE")
  
  # Embedding files doc: https://emscripten.org/docs/porting/files/packaging_files.html#packaging-files
  add_custom_command(
    OUTPUT ${EXECUTABLE_NAME}.html ${EXECUTABLE_NAME}.js ${EXECUTABLE_NAME}.wasm
    COMMAND rm -rf ${DIST_DIR}
    COMMAND mkdir  ${DIST_DIR}
    COMMAND ${EMCC} ${CPP_SOURCE} --use-port=sdl2 --use-port=sdl2_ttf --embed-file=${PROJECT_ROOT_DIR}/${FONT_RELATIVE_PATH}@/${FONT_RELATIVE_PATH} -o ${DIST_DIR}/${EXECUTABLE_NAME}.html
    VERBATIM
  )
  add_custom_target(CGameSandbox
    ALL DEPENDS
    ${EXECUTABLE_NAME}.html
    ${EXECUTABLE_NAME}.js
    ${EXECUTABLE_NAME}.wasm
  )

endif()

