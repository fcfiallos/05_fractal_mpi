#include "fractal_mpi.h"
#include "palette.h"

#include <complex>
#include <cstdint>
extern int max_iterations;
extern std::complex<double> c;

void julia_mpi(double x_min, double y_min, double x_max, double y_max,
                    int rows_start, int rows_end, uint32_t *pixel_buffer)
{
    double dx = (x_max - x_min) / (WIDTH);
    double dy = (y_max - y_min) / (HEIGHT);

    // recorrer cada pixel de la imagen
    for (int j = rows_start; j < rows_end; j++)
    {
        for (int i = 0; i < WIDTH; i++)
        {
            double x = x_min + i * dx;
            double y = y_min + j * dy;

            auto color = divergente(x, y); // auto es igual a var --> inferencia de tipos

            // el reajuste para que sepa la posicion en el buffer desde donde debe iniciar
            pixel_buffer[(j - rows_start) * WIDTH + i] = color; // asignamos el color al pixel
        }
    }

    for (int i = 0; i < WIDTH; i++)
    {
        pixel_buffer[i] = 0xFF000000; // color rojo
    }
}

uint32_t divergente(double x, double y)
{

    /**
     * los valores dados son c y Zo
     * Zn+1 = zN^2 + c -- > lo que buscamos
     * c = -0.7 + 0.27015i ---> viene del main
     */
    int iter = 1;
    double zr = x;
    double zi = y;

    while ((zr * zr + zi * zi) < 4.0 && iter < max_iterations)
    {
        double dr = zr * zr - zi * zi + c.real();
        double di = 2.0 * zr * zi + c.imag();

        zr = dr;
        zi = di;

        iter++;
    }

    if (iter < max_iterations)
    {
        int index = iter % PALETTE_SIZE;
        return color_ramp[index];
        // return 0xFF0000FF; color rojo
    }

    return 0xFF000000;
}