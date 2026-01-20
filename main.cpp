#include <iostream>
#include <fmt/core.h>
#include <mpi.h>
#include "fractal_mpi.h"
#include "palette.h"
#include <complex>
#include <cstdint>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "draw_text.h"
#include "arial_ttf.h"

// en ves de usar fila de matrices s eusan filas de pixeles
//  arreglo de h*ancho de pixeles * alto de pixeles
//  no necesitan enviar buffer del rank 0 a los demas
//  cada proceso calcula su parte y al final se junta todo en el rank 0
int max_iterations = 10;

double x_min = -1.5;
double x_max = 1.5;
double y_min = -1.0;
double y_max = 1.0;

std::complex<double> c(-0.7, 0.27015);

uint32_t *pixel_buffer = nullptr;
uint32_t *texture_buffer = nullptr;

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int nprocs;
    int rank;

    // -ranks: identificador unico de cada proceso
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int delta = std::ceil(HEIGHT * 1.0 / nprocs);

    int rows_start = rank * delta;
    int rows_end = (rank + 1) * delta;

    int padding = delta * nprocs - HEIGHT; // si es exacto da 0 y sino reajusta
    if (rows_end > HEIGHT)
    {
        rows_end = HEIGHT; // para que no se pase del limite
    }
    // ya esta multiplicado por 4
    pixel_buffer = new uint32_t[WIDTH * delta];
    std::memset(pixel_buffer, 0, WIDTH * delta * sizeof(uint32_t));

    fmt::print("Rank {}, nprocs {}, delta {}, rows_start {}, rows_end {}\n", rank, nprocs, delta, rows_start, rows_end);
    // cuando nos aseguramos que los calculos estan y que el rank es 0
    // no va a dibujar la imagen entera
    if (rank == 0)
    {
        texture_buffer = new uint32_t[WIDTH * HEIGHT];
        std::memset(texture_buffer, 0, WIDTH * HEIGHT * sizeof(uint32_t));

        julia_mpi(x_min, y_min, x_max, y_max, rows_start, rows_end, pixel_buffer);

        std::memcpy(texture_buffer, pixel_buffer, WIDTH * delta * sizeof(uint32_t));

        for (int i = 1; i < nprocs; i++)
        {
            MPI_Recv(
                texture_buffer + i * WIDTH * delta,     // este es el puntero
                WIDTH * delta,                          // no puede recivir mas de lo que envio
                MPI_UNSIGNED,                           // dibujo entero sin signo y esto se envia y en donde recibo
                i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE // envia
            );
        }
        //escribir imagen
        stbi_write_png("fractal.png", WIDTH, HEIGHT, STBI_rgb_alpha, texture_buffer, WIDTH * 4);
    }
    else
    {
        julia_mpi(x_min, y_min, x_max, y_max, rows_start, rows_end, pixel_buffer); // cada uno ya hizo su parte ahora se debe unir todo (enviar)
        // auto text = fmt::format("Rank {}", rank);

        // draw_text_to_texture(
        //     (unsigned char*)pixel_buffer, WIDTH, delta,
        //     text.c_str(), 10,25,20);

        MPI_Send(
            (unsigned char*)pixel_buffer, WIDTH * delta, MPI_UNSIGNED, // dibujo entero sin signo y esto se envia
            0, 0, MPI_COMM_WORLD                       // recibe
        );
    }

    std::cout.flush(); // obliga a escribir en cmd

    MPI_Finalize();
    return 0;
}