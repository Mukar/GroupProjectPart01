#pragma once
#ifndef VECTOR3
#define VECTOR3

struct Vector3
{
    float x;
    float y;
    float z;

public:
    Vector3() //Basic constructor
    {
    x = 0;
    y = 0;
    z = 0;
    }

    Vector3(int newX, int newY, int newZ) //Constructor using integers
    {
    x = newX;
    y = newY;
    z = newZ;
    }

    Vector3(float newX, float newY, float newZ) //Constructor using floats
    {
    x = newX;
    y = newY;
    z = newZ;
    }
};

#endif
