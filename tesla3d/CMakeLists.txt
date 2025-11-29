cmake_minimum_required(VERSION 3.5)
project(tesla3d)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -fno-exceptions -fno-rtti -flto")

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBDRM REQUIRED libdrm)
pkg_check_modules(GBM REQUIRED gbm)
pkg_check_modules(EGL REQUIRED egl)
pkg_check_modules(GLESv2 REQUIRED glesv2)

include_directories(
  ${LIBDRM_INCLUDE_DIRS}
  ${GBM_INCLUDE_DIRS}
  ${EGL_INCLUDE_DIRS}
  ${GLESv2_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/extern
)

link_directories(${LIBDRM_LIBRARY_DIRS} ${GBM_LIBRARY_DIRS} ${EGL_LIBRARY_DIRS} ${GLESv2_LIBRARY_DIRS})

add_executable(tesla3d
  src/main.cpp
  src/gl_helpers.cpp
  src/tiny_gltf_wrapper.cpp
)

target_link_libraries(tesla3d
  ${LIBDRM_LIBRARIES}
  ${GBM_LIBRARIES}
  ${EGL_LIBRARIES}
  ${GLESv2_LIBRARIES}
  m
)
