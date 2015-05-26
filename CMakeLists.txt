cmake_minimum_required(VERSION 2.8)

PROJECT(piroverc)

SET(piroverc_SOURCES

  main.cpp
  control.cpp
  net.cpp
  videowindow.cpp

)

INCLUDE(FindPkgConfig)

PKG_CHECK_MODULES (DEPS REQUIRED glib-2.0 gio-2.0 gstreamer-1.0 gtk+-3.0 sdl2)
INCLUDE_DIRECTORIES(${DEPS_INCLUDE_DIRS})
LINK_DIRECTORIES(${DEPS_LIBRARY_DIRS})

ADD_EXECUTABLE(${PROJECT_NAME} ${piroverc_SOURCES})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${DEPS_LIBRARIES} -lgstvideo-1.0 ${DEPS_LDFLAGS_OTHER})
