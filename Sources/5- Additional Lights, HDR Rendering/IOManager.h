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
    static bool ReadJpg(const char *filename, unsigned int width, unsigned int height, unsigned char *buffer);

    static bool WritePng(const char *filename, unsigned int width, unsigned int height, unsigned char *buffer);
    static bool WritePpm(const char *filename, unsigned int width, unsigned int height, unsigned char *buffer);
    static bool WriteExr(const char *filename, unsigned int width, unsigned int height, unsigned char *buffer);

private:


};

#endif