/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __RANDOMGENERATOR_H__
#define __RANDOMGENERATOR_H__

class RandomGenerator
{
public:
    // Return random float in [0, 1]
    static float GetRandomFloat();

    // Return random float in [0, max]
    static float GetRandomFloat(float max);

    // Return random float in [min, max]
    static float GetRandomFloat(float min, float max);

    // Return random integer in [-MAX_INT, MAX_INT]
    static int GetRandomInt();

    // Return random integer in [0, max]
    static int GetRandomInt(int max);

    // Return random integer in [min, max]
    static int GetRandomInt(int min, int max);

    // Return random unsigned integer in [0, MAX_UINT]
    static unsigned int GetRandomUInt();

    // Return random unsigned integer in [0, max]
    static unsigned int GetRandomUInt(unsigned int max);

    // Return random unsigned integer in [min, max]
    static unsigned int GetRandomUInt(unsigned int min, unsigned int max);

//    static bool UnitTest();
private:

};

#endif
