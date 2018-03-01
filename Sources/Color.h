#ifndef __COLOR_H__
#define __COLOR_H__

class Colori
{
public:
    Colori();
    Colori(unsigned char red, unsigned char green, unsigned char blue) 
    : r(red), g(green), b(blue)
    {}

    ~Colori();

    unsigned char r, g, b;
private:

};

class Colorf
{
public:
    Colorf();
    Colorf(float red, float green, float blue) 
    : r(red), g(green), b(blue)
    {}

    ~Colorf();

    float r, g, b;
private:

};

#endif