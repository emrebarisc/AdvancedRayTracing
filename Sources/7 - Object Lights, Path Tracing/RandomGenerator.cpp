/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "RandomGenerator.h"

#include <random>

#include "Math.h"

std::random_device randomDevice;  //Will be used to obtain a seed for the random number engine
std::mt19937 randomGenerator(randomDevice()); //Standard mersenne_twister_engine seeded with randomDevice()
std::uniform_real_distribution<float> uniformDistribution(0.0, 1.0);

// Return random value in [0, 1]
float RandomGenerator::GetRandomFloat()
{
    return uniformDistribution(randomGenerator);
}

// Return random value in [0, max]
float RandomGenerator::GetRandomFloat(float max)
{
    return GetRandomFloat() * max;
}

// Return random value in [min, max]
float RandomGenerator::GetRandomFloat(float min, float max)
{
    return GetRandomFloat() * (max - min) + min;
}

// Return random value in [-MAX_INT, MAX_INT]
int RandomGenerator::GetRandomInt()
{
    return (int)((uniformDistribution(randomGenerator) - 0.5f) * 2 * MAX_INT);
}

// Return random value in [0, max]
int RandomGenerator::GetRandomInt(int max)
{
    int randomValue = GetRandomInt();
    return randomValue < max ? randomValue : randomValue % max;
}

// Return random value in [min, max]
int RandomGenerator::GetRandomInt(int min, int max)
{
    return GetRandomInt() % (max - min) + min;
}

// Return random value in [0, MAX_UINT]
unsigned int RandomGenerator::GetRandomUInt()
{
    return (unsigned int)(uniformDistribution(randomGenerator) * MAX_UINT);
}

// Return random unsigned integer in [0, max]
unsigned int RandomGenerator::GetRandomUInt(unsigned int max)
{
    int randomValue = GetRandomUInt();
    return randomValue < max ? randomValue : randomValue % max;
}

// Return random unsigned integer in [min, max]
unsigned int RandomGenerator::GetRandomUInt(unsigned int min, unsigned int max)
{
    return GetRandomUInt() % (max - min) + min;
}

// RandomGenerator unit test
bool RandomGenerator::UnitTest()
{
    for(unsigned int i = 0; i < 1000; i++)
    {
        float randomFloat = RandomGenerator::GetRandomFloat();
        if(randomFloat < 0 || randomFloat > 1)
        {
            std::cout << "Error in GetRandomFloat." << std::endl;
            return false;
        }

        srand(time(NULL));

        float min = (float)rand()/(float)(RAND_MAX);
        float max = min + (float)rand()/(float)(RAND_MAX);
        randomFloat = RandomGenerator::GetRandomFloat(max);
        if(randomFloat < 0 || randomFloat > max)
        {
            std::cout << "Error in GetRandomFloat(max)." << std::endl;
            return false;
        }

        randomFloat = RandomGenerator::GetRandomFloat(min, max);
        if(randomFloat < min || randomFloat > max)
        {
            std::cout << "Error in GetRandomFloat(min, max)." << std::endl;
            return false;
        }

        int randomInt = RandomGenerator::GetRandomInt();
        if(randomInt < -MAX_INT || randomInt > MAX_INT)
        {
            std::cout << "Error in GetRandomInt. " << randomInt << std::endl;
            return false;
        }

        randomInt = RandomGenerator::GetRandomInt((int)max);
        if(randomInt < -MAX_INT || randomInt > (int)max)
        {
            std::cout << "Error in GetRandomInt(max). " << randomInt << std::endl;
            return false;
        }

        randomInt = RandomGenerator::GetRandomInt((int)min, (int)max);
        if(randomInt < (int)min || randomInt > (int)max)
        {
            std::cout << "Error in GetRandomInt(min, max). " << randomInt << std::endl;
            return false;
        }

        unsigned int randomUInt = RandomGenerator::GetRandomUInt();
        if(randomUInt < 0 || randomUInt > MAX_UINT)
        {
            std::cout << "Error in GetRandomUInt. " << randomUInt << std::endl;
            return false;
        }

        randomUInt = RandomGenerator::GetRandomUInt((unsigned int)max);
        if(randomUInt < 0 || randomUInt > (unsigned int)max)
        {
            std::cout << "Error in GetRandomUInt(max). " << randomUInt << std::endl;
            return false;
        }

        randomUInt = RandomGenerator::GetRandomUInt((unsigned int)min, (unsigned int)max);
        if(randomUInt < (unsigned int)min || randomUInt > (unsigned int)max)
        {
            std::cout << "Error in GetRandomUInt(min, max). " << randomUInt << std::endl;
            return false;
        }
    }

    return true;
}