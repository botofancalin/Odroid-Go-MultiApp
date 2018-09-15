#pragma once
#include "odroid_go.h"

class CfgBrightnessClass
{
public:
    CfgBrightnessClass();
    ~CfgBrightnessClass();

    void Run();

private:
    unsigned short tmp_bright;
    unsigned short tmp_lbright;
    int val;
};