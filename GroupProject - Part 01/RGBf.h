#pragma once
#ifndef rgbf
#define rgbf

struct RGBf
{
    float red;
    float green;
    float blue;

public:
    RGBf(float newR, float newG, float newB) //Basic Constructor where the magic happens.
    {
        red = newR/255;
        green = newG/255;
        blue = newB/255;
    }
};

#endif
