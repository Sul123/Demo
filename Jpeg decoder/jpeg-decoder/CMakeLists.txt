cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 17)
project(jpeg-decoder)

add_executable(jpeg_decoder
        main.cpp
        decoder.cpp
        errors.cpp
        bit_reader.cpp
        huffman.cpp)

target_link_libraries(jpeg_decoder fftw3 png)
