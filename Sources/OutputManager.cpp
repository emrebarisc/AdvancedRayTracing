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

void SetRGB(png_byte *ptr, float val)
{
	int v = (int)(val * 767);
	if (v < 0) v = 0;
	if (v > 767) v = 767;
	int offset = v % 256;

	if (v<256)
    {
		ptr[0] = 0; ptr[1] = 0; ptr[2] = offset;
	}
	else if (v<512)
    {
		ptr[0] = 0; ptr[1] = offset; ptr[2] = 255-offset;
	}
	else
    {
		ptr[0] = offset; ptr[1] = 255-offset; ptr[2] = 0;
	}
}

bool OutputManager::WritePng(const char *filename, int width, int height, float *buffer)
{
    bool outVal = false;
    FILE *fp = NULL;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_bytep row = NULL;

    bool finishOperation = false;

    // Open file for writing (binary mode)
    fp = fopen(filename, "wb");
    if (fp == NULL) 
    {
        fprintf(stderr, "Could not open file %s for writing\n", filename);
        outVal = true;
        finishOperation = true;
    }

    // Initialize write structure
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
    {
        fprintf(stderr, "Could not allocate write struct\n");
        outVal = true;
        finishOperation = true;
    }

    // Initialize info structure
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        fprintf(stderr, "Could not allocate info struct\n");
        outVal = true;
        finishOperation = true;
    }

    // Setup Exception handling
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        fprintf(stderr, "Error during png creation\n");
        outVal = true;
        finishOperation = true;
    }

    if(!finishOperation)
    {
        png_init_io(png_ptr, fp);

        // Write header (8 bit colour depth)
        png_set_IHDR(png_ptr, info_ptr, width, height,
            8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        png_write_info(png_ptr, info_ptr);

        // Allocate memory for one row (3 bytes per pixel - RGB)
        row = (png_bytep) malloc(3 * width * sizeof(png_byte));

        // Write image data
        int x, y;
        for (y=0 ; y<height ; y++) 
        {
            for (x=0 ; x<width ; x++) 
            {
                SetRGB(&(row[x*3]), buffer[y*width + x]);
            }
            png_write_row(png_ptr, row);
        }

        // End write
        png_write_end(png_ptr, NULL);
    }
    
    if (fp != NULL) fclose(fp);
    if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    if (row != NULL) free(row);

    return outVal;
}

bool OutputManager::WritePpm(const char* filename, int width, int height, float* buffer)
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