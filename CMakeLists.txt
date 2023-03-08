# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.16)

# project name and version
project(Alsa_Callbacks VERSION 0.1)

# set cpp standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(alsa_callbacks STATIC
    src/callback.cc
)

target_link_libraries(alsa_callbacks asound)
target_link_libraries(alsa_callbacks pthread)

# set props
# set_target_properties(alsa_callbacks PROPERTIES VERSION ${PROJECT_VERSION})

add_executable(alsa_callbacks_test test/test.cc)
target_link_libraries(alsa_callbacks_test alsa_callbacks)
