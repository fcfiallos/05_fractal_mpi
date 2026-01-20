cmake_minimum_required(VERSION 3.16)

project(FractalMPI)

find_package(fmt CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(Freetype REQUIRED)
find_package(SFML COMPONENTS Network Graphics Window Audio System CONFIG REQUIRED)

#config MPI
set(MPI_ROOT "c:/oneAPI/mpi/latest")
set(MPI_INCLUDE "${MPI_ROOT}/include")
set(MPI_LIB "${MPI_ROOT}/lib")

include_directories(${MPI_INCLUDE})
include_directories(${Stb_INCLUDE_DIR}) # tenemos ddentro de esa carpeta extandar , estandar de C++/C


add_executable(FractalMPI main.cpp fractal_mpi.cpp palette.cpp draw_text.cpp)

target_link_libraries(FractalMPI PRIVATE
    fmt::fmt
    ${MPI_LIB}/impi.lib # errores en el .h si no encuentra esa configuracion
    Freetype::Freetype)

add_executable(fractal main_fractal.cpp fractal_mpi.cpp palette.cpp draw_text.cpp)

target_link_libraries(fractal PRIVATE
    fmt::fmt
    ${MPI_LIB}/impi.lib
    Freetype::Freetype
    SFML::Network SFML::Graphics SFML::Window SFML::Audio SFML::System) # errores en el .h si no encuentra esa configuracion

add_executable(fractal_colectiva main_fractal_colectiva.cpp fractal_mpi.cpp palette.cpp draw_text.cpp)

target_link_libraries(fractal_colectiva PRIVATE
    fmt::fmt
    ${MPI_LIB}/impi.lib
    Freetype::Freetype
    SFML::Network SFML::Graphics SFML::Window SFML::Audio SFML::System) # errores en el .h si no encuentra esa configuracion
