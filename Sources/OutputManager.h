/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __OUTPUTMANAGER_H__
#define __OUTPUTMANAGER_H__

class OutputManager
{
public:

    static bool WritePng(const char *filename, int width, int height, float *buffer);

    static bool WritePpm(const char* filename, int width, int height, float* buffer);

private:


};

#endif