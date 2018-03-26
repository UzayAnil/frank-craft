#include "Util.hpp"
#include <chrono>
#include <iostream>
#include <glm/glm.hpp>
using namespace std::chrono;
using namespace std;
using namespace glm;

double lerp(double t, double a, double b) { return a + t * (b - a); }

double getTime() {
    return duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()).count();
}

string getAssetFilePath(const char *base) {
    return string("./Assets/") + string(base);
}
