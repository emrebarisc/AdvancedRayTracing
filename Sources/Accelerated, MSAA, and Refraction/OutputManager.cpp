/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "OutputManager.h"

#include <cstdlib>
#include <stdexcept>
#include <png.h>

#include <iostream>

bool OutputManager::WritePng(const char *filename, unsigned int width, unsigned int height, unsigned char *buffer)
{
    // Create our PNG file
    FILE *file = fopen(filename, "wb");
    if(!file) 
    {
        std::cerr << "Error opening the PNG file." << std::endl;
        return false;
    }

    // Initialize
    png_structp pngPointer = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!pngPointer)
    {
        std::cerr << "Error creating png write structure." << std::endl;
        return false;
    }

    png_infop infoPointer = png_create_info_struct(pngPointer);
    if(!infoPointer)
    {
        std::cerr << "Error creating png info structure." << std::endl;
        return false;
    }

    if(setjmp(png_jmpbuf(pngPointer)))
    {
        std::cerr << "Error during init_io." << std::endl;
        return false;
    }

    // Write
    png_init_io(pngPointer, file);
    png_set_IHDR(   pngPointer, infoPointer, width, height,
                    8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                    PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(pngPointer, infoPointer);

    unsigned int colorIndex = 0;
    png_bytep *rowPointers = (png_bytep *) malloc(height * sizeof(png_bytep));
    for(unsigned int y = 0; y < height; y++)
    {
        rowPointers[y] = (png_byte *) malloc(width * 3 * sizeof(png_byte));
        png_byte *row = rowPointers[y];

        for(unsigned int x = 0; x < width; x++)
        {
            *row++ = buffer[colorIndex++];
            *row++ = buffer[colorIndex++];
            *row++ = buffer[colorIndex++];
        }
    }

    png_set_rows(pngPointer, infoPointer, rowPointers);
    png_write_png(pngPointer, infoPointer, PNG_TRANSFORM_IDENTITY, NULL);

    // Finish writing
    if (setjmp(png_jmpbuf(pngPointer)))
    {
        std::cerr << "Error during finishing writing." << std::endl;
        return false;
    }

    png_write_end(pngPointer, NULL);

    /* cleanup heap allocation */
    for (unsigned int y = 0; y < height; y++)
        free(rowPointers[y]);
    
    free(rowPointers);
    fclose(file);

    return true;
}

bool OutputManager::WritePpm(const char* filename, unsigned int width, unsigned int height, unsigned char* buffer)
{
    FILE *outfile;

    std::string ppmFileName(filename);
    ppmFileName.append(".ppm");

    if ((outfile = fopen(ppmFileName.c_str(), "w")) == NULL) 
    {
        std::cerr << "Error: The ppm file could be opened for writing." << std::endl;
        return false;
    }

    (void) fprintf(outfile, "P3\n%d %d\n255\n", width, height);

    unsigned char color;
    for (size_t j = 0, idx = 0; j < height; ++j)
    {
        for (size_t i = 0; i < width; ++i)
        {
            for (size_t c = 0; c < 3; ++c, ++idx)
            {
                color = (unsigned char)(int)buffer[idx];

                if (i == width - 1 && c == 2)
                {
                    (void) fprintf(outfile, "%d", color);
                }
                else
                {
                    (void) fprintf(outfile, "%d ", color);
                }
            }
        }

        (void) fprintf(outfile, "\n");
    }

    (void) fclose(outfile);

    return true;
}