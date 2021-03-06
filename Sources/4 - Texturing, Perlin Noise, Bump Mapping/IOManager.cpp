/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "IOManager.h"

#include <cstdlib>
#include <stdexcept>
#include <png.h>
#include <jpeglib.h>

#include <iostream>

void IOManager::GetJpgSize(const char *filename, int &width, int &height)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	FILE *infile;

	/* create jpeg decompress object */
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	/* set input file name */
	if ((infile = fopen(filename, "rb")) == NULL) 
    {
		fprintf(stderr, "Can't open %s\n", filename);
		exit(1);
	}

	jpeg_stdio_src(&cinfo, infile);

	/* read header */
	jpeg_read_header(&cinfo, TRUE);

	jpeg_start_decompress(&cinfo);
	width = cinfo.output_width;
	height = cinfo.output_height;
}

bool IOManager::ReadJpg(const char *filename, unsigned int width, unsigned int height, unsigned char *buffer)
{	
    struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	FILE *infile;
	unsigned char *row_pointer; /* pointer to a row */
	unsigned int j, k;

	/* create jpeg decompress object */
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	/* set input file name */
	if ((infile = fopen(filename, "rb")) == NULL) 
    {
		std::cout << "Can't open " << filename << std::endl;
        return false;
	}

	jpeg_stdio_src(&cinfo, infile);

	/* read header */
	jpeg_read_header(&cinfo, TRUE);

	jpeg_start_decompress(&cinfo);
	if (width != cinfo.output_width || height != cinfo.output_height)
    {
        std::cout << "Error: Actual JPEG resolution does not match the provided one." << std::endl;
        return false;
    }

    row_pointer = (unsigned char *) malloc(sizeof(unsigned char) * width * 3);

	while (cinfo.output_scanline < cinfo.output_height) 
    {
		jpeg_read_scanlines(&cinfo, &row_pointer, 1);
        unsigned int startY = (cinfo.output_scanline - 1) * width * 3;
    
		for(j = 0; j < width; j++) 
        {
            unsigned int startX = j * 3;
                
            if(cinfo.jpeg_color_space == J_COLOR_SPACE::JCS_GRAYSCALE)
            {
                buffer[startY + startX    ] = (unsigned char) row_pointer[j];
                buffer[startY + startX + 1] = (unsigned char) row_pointer[j];
                buffer[startY + startX + 2] = (unsigned char) row_pointer[j];
            }
            else
            {
                buffer[startY + startX    ] = (unsigned char) row_pointer[startX    ];
                buffer[startY + startX + 1] = (unsigned char) row_pointer[startX + 1];
                buffer[startY + startX + 2] = (unsigned char) row_pointer[startX + 2];
            }
        }
	}

	jpeg_finish_decompress(&cinfo);
	fclose(infile);
	free(row_pointer);
	jpeg_destroy_decompress(&cinfo);

    return true;
}

bool IOManager::WritePng(const char *filename, unsigned int width, unsigned int height, unsigned char *buffer)
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

bool IOManager::WritePpm(const char* filename, unsigned int width, unsigned int height, unsigned char* buffer)
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