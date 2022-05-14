cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)

project(tickit)

add_library(tickit 
  src/bindings.c
  src/debug.c
  src/evloop-default.c
  src/mockterm.c
  src/pen.c
  src/rect.c
  src/rectset.c
  src/renderbuffer.c
  src/string.c
  src/term.c
  src/termdriver-ti.c
  src/termdriver-xterm.c
  src/tickit.c
  src/utf8.c
  src/window.c)

target_link_libraries(tickit PUBLIC termkey unibilium )
add_definitions(-DHAVE_UNIBILIUM=1)

target_include_directories(tickit PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  )
