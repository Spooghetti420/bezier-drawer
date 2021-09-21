#include "olcPixelGameEngine.h"

// Finish creating slider object to represent a slider.
// To be used to allow the user to specify a detail value for drawing the Beziers
class Slider {
    Slider(double _from, double _to, double _increment, double _startingValue, olc::vd2d _position) {
        double from = _from;
        double to = _to;
        double increment = _increment;
        double value = _startingValue;
        olc::vd2d pos = _position;
    }

    void Draw() {
        olc::PixelGameEngine::FillRect(this->from, )
    }
};