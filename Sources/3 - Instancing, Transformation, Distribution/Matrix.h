/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "Math.h"

class Matrix
{
public:
    Matrix()
    {
        for (int i = 0; i < 16; i++)
        {
            m[i] = 0;
        }
    }

    Matrix(float value)
    {
        for (int i = 0; i < 16; i++)
        {
            m[i] = value;
        }
    }

    Matrix(float val1, float val2, float val3, float val4,
           float val5, float val6, float val7, float val8, 
           float val9, float val10, float val11, float val12, 
           float val13, float val14, float val15, float val16)
    {
        m[0] = val1;
        m[1] = val2;
        m[2] = val3;
        m[3] = val4;

        m[4] = val5;
        m[5] = val6;
        m[6] = val7;
        m[7] = val8;

        m[8] = val9;
        m[9] = val10;
        m[10] = val11;
        m[11] = val12;

        m[12] = val13;
        m[13] = val14;
        m[14] = val15;
        m[15] = val16;  
    }

    Matrix(const float matrix[16])
    {
        for (int i = 0; i < 16; i++)
        {
            m[i] = matrix[i];
        }
    }

    Matrix(const Matrix& other)
    {
        if(this != &other)
        {
            for (int i = 0; i < 16; i++)
            {
                m[i] = other.m[i];
            }
        }
    }

    ~Matrix()
    {

    }

    void operator=(const Matrix& rhs)
    {
        if(this != &rhs)
        {
            for (int i = 0; i < 16; i++)
            {
                m[i] = rhs.m[i];
            }
        }
    }

    Vector4 operator*(const Vector4& rhs) const
    {
        Vector4 out(0);

        out.x = m[0] * rhs.x + m[1] * rhs.y + m[2] * rhs.z + m[3] * rhs.w;
        out.y = m[4] * rhs.x + m[5] * rhs.y + m[6] * rhs.z + m[7] * rhs.w;
        out.z = m[8] * rhs.x + m[9] * rhs.y + m[10] * rhs.z + m[11] * rhs.w;
        out.w = m[12] * rhs.x + m[13] * rhs.y + m[14] * rhs.z + m[15] * rhs.w;

        return out;
    }

    Matrix operator*(const Matrix& rhs) const
    {
        Matrix out(0.f);

        out.m[0] = m[0] * rhs.m[0] + m[1] * rhs.m[4] + m[2] * rhs.m[8] + m[3] * rhs.m[12];
        out.m[1] = m[0] * rhs.m[1] + m[1] * rhs.m[5] + m[2] * rhs.m[9] + m[3] * rhs.m[13];
        out.m[2] = m[0] * rhs.m[2] + m[1] * rhs.m[6] + m[2] * rhs.m[10] + m[3] * rhs.m[14];
        out.m[3] = m[0] * rhs.m[3] + m[1] * rhs.m[7] + m[2] * rhs.m[11] + m[3] * rhs.m[15];

        out.m[4] = m[4] * rhs.m[0] + m[5] * rhs.m[4] + m[6] * rhs.m[8] + m[7] * rhs.m[12];
        out.m[5] = m[4] * rhs.m[1] + m[5] * rhs.m[5] + m[6] * rhs.m[9] + m[7] * rhs.m[13];
        out.m[6] = m[4] * rhs.m[2] + m[5] * rhs.m[6] + m[6] * rhs.m[10] + m[7] * rhs.m[14];
        out.m[7] = m[4] * rhs.m[3] + m[5] * rhs.m[7] + m[6] * rhs.m[11] + m[7] * rhs.m[15];

        out.m[8] = m[8] * rhs.m[0] + m[9] * rhs.m[4] + m[10] * rhs.m[8] + m[11] * rhs.m[12];
        out.m[9] = m[8] * rhs.m[1] + m[9] * rhs.m[5] + m[10] * rhs.m[9] + m[11] * rhs.m[13];
        out.m[10] = m[8] * rhs.m[2] + m[9] * rhs.m[6] + m[10] * rhs.m[10] + m[11] * rhs.m[14];
        out.m[11] = m[8] * rhs.m[3] + m[9] * rhs.m[7] + m[10] * rhs.m[11] + m[11] * rhs.m[15];

        out.m[12] = m[12] * rhs.m[0] + m[13] * rhs.m[4] + m[14] * rhs.m[8] + m[15] * rhs.m[12];
        out.m[13] = m[12] * rhs.m[1] + m[13] * rhs.m[5] + m[14] * rhs.m[9] + m[15] * rhs.m[13];
        out.m[14] = m[12] * rhs.m[2] + m[13] * rhs.m[6] + m[14] * rhs.m[10] + m[15] * rhs.m[14];
        out.m[15] = m[12] * rhs.m[3] + m[13] * rhs.m[7] + m[14] * rhs.m[11] + m[15] * rhs.m[15];

        return out;
    }

    float operator[](int index)
    {
        if(index >= 0 && index <= 15)
        {
            return m[index];
        }
    }

    bool operator==(const Matrix &rhs)
    {
        for(unsigned int i = 0; i < 16; i++)
        {
            if(m[i] != rhs.m[i]) return false;
        }

        return true;
    }


    inline friend std::ostream& operator<<(std::ostream& out, const Matrix& matrix)
    {
        return out << "|" << matrix.m[0] << ", " << matrix.m[1] << ", " << matrix.m[2] << ", " << matrix.m[3] << "|" << std::endl
                   << "|" << matrix.m[4] << ", " << matrix.m[5] << ", " << matrix.m[6] << ", " << matrix.m[7] << "|" << std::endl
                   << "|" << matrix.m[8] << ", " << matrix.m[9] << ", " << matrix.m[10] << ", " << matrix.m[11] << "|" << std::endl
                   << "|" << matrix.m[12] << ", " << matrix.m[13] << ", " << matrix.m[14] << ", " << matrix.m[15] << "|" << std::endl;
    }

    Matrix GetInverse() const;
    Matrix GetTranspose() const;
    Matrix GetUpper3x3() const;

    static const Matrix ZeroMatrix;
    static const Matrix IdentityMatrix;

    float m[16];
};


#endif