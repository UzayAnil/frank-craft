#include "Util.hpp"
#include <chrono>
using namespace std::chrono;

double lerp(double t, double a, double b) { return a + t * (b - a); }

double getTime() {
    return duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()).count();
}
