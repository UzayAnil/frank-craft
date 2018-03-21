#include <iostream>
using namespace std;

#include "Terrain.hpp"
constexpr int ImprovedNoise::p[512];

double octiveNoise( double x, double y, int layers, double persist ) {
    double ret = 0;
    double amp = 1;
    double max = 0;

    static double min = 0;

    for ( int i = 0; i < layers; i++ ) {
        ret += amp * ImprovedNoise::noise( x, y, i );
        max += amp;
        amp *= persist;
    }
    return (ret+max)/(2*max);
}
