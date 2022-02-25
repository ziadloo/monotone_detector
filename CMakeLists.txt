cmake_minimum_required(VERSION 3.21)
project(monotone_detector)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "/usr/include/qt;/usr/include/qwt")

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
message(STATUS "QWT_INCLUDE_DIR: ${QWT_INCLUDE_DIR}")
message(STATUS "QWT_LIBRARY: ${QWT_LIBRARY}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(monotone_detector main.cpp
        FrequencyPlotter.hpp
        pc/producer.cpp
        pc/producer.h
        pc/consumer.cpp
        pc/consumer.h
        utility.h
        pc/task.cpp
        pc/task.h
        operations/print_similarity.hpp
        operations/buffer_and_save.hpp
        operations/noise_detector.hpp
        operations/operation.cpp
        operations/operation.h
        operations/raw_operation.cpp
        operations/raw_operation.h)

target_include_directories(monotone_detector PRIVATE "/usr/include;/usr/include/qwt")
target_link_libraries(monotone_detector PRIVATE Qt5::Core Qt5::Widgets "/usr/lib/libqwt.so")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(monotone_detector PRIVATE Threads::Threads)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/binary/rtaudio/include/rtaudio)
target_link_libraries(monotone_detector PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/binary/rtaudio/lib/librtaudio.a)
set(LINKLIBS)
list(APPEND LINKLIBS ${ALSA_LIBRARY})
list(APPEND INCDIRS ${ALSA_INCLUDE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/binary/fftw/include)
target_link_libraries(monotone_detector PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/binary/fftw/lib/libfftw3.a)

target_link_libraries(monotone_detector PRIVATE ${LINKLIBS})
target_link_libraries(monotone_detector PRIVATE jack)
target_link_libraries(monotone_detector PRIVATE /usr/lib/libasound.so)
target_link_libraries(monotone_detector PRIVATE /usr/lib/libpulse.so)
target_link_libraries(monotone_detector PRIVATE /usr/lib/libpulse-simple.so)
