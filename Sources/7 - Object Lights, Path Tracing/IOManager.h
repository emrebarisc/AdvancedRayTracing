/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __IOMANAGER_H__
#define __IOMANAGER_H__

class IOManager
{
public:

    static void GetJpgSize(const char *filename, int &width, int &height);
    static bool ReadJpg(const char *filename, int width, int height, unsigned char *buffer);
    static bool ReadExr(const char *filename, int &width, int &height, float *buffer);

    static bool WritePng(const char *filename, int width, int height, float *buffer);
    static bool WritePpm(const char *filename, int width, int height, unsigned char *buffer);
    static bool WriteExr(const char *filename, int width, int height, float *buffer);

private:


};

#endif