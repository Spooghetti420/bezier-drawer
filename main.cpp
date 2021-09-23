#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <iostream>
#include <math.h>

/* 
TODO:

Move the various factorial, binomial coefficient, etc. helper functions to a
separate file to decrease the clutter, maybe initialise the variables elsewhere too
but idk.

*/


const int width = 1920;
const int height = 1080;
const bool fullscreen = true;
const double deletionDistance = 50*50; // The distance within which a point is deleted if right-clicked
const double draggingDistance = 25*25; // The distance within which a point is dragged instead of creating a new one

class Interpolation : public olc::PixelGameEngine
{
public:
    std::vector<olc::vd2d> points = {};
    int selectedPoint;
    bool moving = false;
    
    int frameCount = 0;
    int bezierAnimationTimer = 100;
    int stepDirection = 1;

    int prevAnimationTimer = bezierAnimationTimer;
    std::vector<olc::vd2d> prevPoints = {};
    olc::vd2d prevMouse = olc::vd2d(0, 0);

    Interpolation()
    {
        sAppName = "Bezier Curve Demo";
        olc::vd2d firstPoint = olc::vd2d(480.0, 480.0); 
        points.push_back(firstPoint);
    }
public:
    bool OnUserCreate() override 
    {

        return true;
    }
    bool OnUserUpdate(float fElapsedTime) override
    {
        int mouseX = GetMouseX();
        int mouseY = GetMouseY();
        olc::vd2d mousePosition = olc::vd2d(mouseX, mouseY);
        olc::HWButton leftMouse = GetMouse(olc::Mouse::LEFT);
        olc::HWButton rightMouse = GetMouse(olc::Mouse::RIGHT);

        if (leftMouse.bPressed) {
            int indexClosest = getClosest(mousePosition, points);
            olc::vd2d closest = points[indexClosest];
            if ((closest-mousePosition).mag2() < draggingDistance) {
                moving = true;
                selectedPoint = indexClosest;
            } else if (points.size() < 11) {
                points.insert(points.end()-1, mousePosition);
                moving = true;
                selectedPoint = points.size()-2;
            }
        } else if (leftMouse.bReleased) {
            moving = false;
        }

        if (rightMouse.bPressed) {
            // std::cout << "Right mouse pressed" << std::endl;
            if (points.size() > 1) {
                int closest = getClosest(mousePosition, points);
                if ((points[closest]-mousePosition).mag2() < deletionDistance) {
                    points.erase(points.begin() + closest);
                    moving = false;
                }
            }
        }

        if (GetKey(olc::Key::SPACE).bPressed) {
            stepDirection *= -1;
        }

        // std::cout << getClosest(mousePosition, points);

        if (moving) {
            points[selectedPoint].x = mouseX;
            points[selectedPoint].y = mouseY;
        }

        frameCount++;
        if (stepDirection == 1) {
            if (bezierAnimationTimer < 100) {
                bezierAnimationTimer++;
            }
        } else {
            if (bezierAnimationTimer > 0) {
                bezierAnimationTimer--;
            }
        }

        if (bezierAnimationTimer == prevAnimationTimer && points == prevPoints && mousePosition == prevMouse) {
            // Skip some rendering if nothing has changed.
            return true;
        }

        Clear(olc::BLACK);
        for (auto p : points) {
            double distance = (p-mousePosition).mag2();
            float radius = 10;
            if (distance < draggingDistance) {
                radius *= 1.5;
            }

            FillCircle(p, radius);
        }

        double t = (bezierAnimationTimer%101)/100.0;
        if (bezierAnimationTimer != 0 && bezierAnimationTimer != 100) {
            FillCircle(interpolateBezier(points, t), 20, olc::RED);
        }
        drawBezier(points, t, 0.01);

        prevAnimationTimer = bezierAnimationTimer;
        prevPoints = points;
        prevMouse = mousePosition;

        return true;
    }
private:
    static int getClosest(olc::vd2d origin, std::vector<olc::vd2d> points) {
        double maximum = INFINITY;
        int closest;
        for (int i = 0; i < points.size(); i++) {
            double dist = (origin-points[i]).mag2();
            if (dist < maximum) {
                maximum = dist;
                closest = i;
            }
        }
    std::cout << closest << std::endl;
    return closest;
    }

    static int factorial(int n) {
        if (n == 0) return 1;
        if (n == 1) return 1;

        int product = 1;
        for (int i = 1; i <= n; i++) {
            product *= i;
        }
        return product;
    }

    static int binomial_coefficient(int n, int k) {
        // std::cout << "n is " << n << "; k is " << k << "\n";
        // std::cout << factorial(n) << " " << factorial(k) << " " << factorial(n-k) << std::endl;
        return factorial(n) / (factorial(k)*factorial(n-k));
    }

    static olc::vd2d interpolateBezier(std::vector<olc::vd2d> points, double t) {
        // Given t from [0, 1], gives the point where the Bezier curve described by a vector of points is located
        // std::cout << "Drawing bezier point" << std::endl;
        int length = points.size();
        olc::vd2d sum = olc::vd2d(0, 0);
        for (int i = 0; i < length; i++) {
            int coefficient = binomial_coefficient(length-1, i);
            // std::cout << "Coefficient is: " << coefficient << "; Power of (1-t) is: " << length-1-i << "; Power of t is: " << i << "; Point is: " << points[i] << std::endl;
            sum += coefficient * pow(1-t, length-1-i) * pow(t, i) * points[i];
        }
        return sum;
    }

    void drawBezier(std::vector<olc::vd2d> points, double t, double increment=0.01) {
        // Draws the Bezier curve for a set of points by joining straight lines at regular intervals along the line
        olc::vd2d prev = interpolateBezier(points, 0);
        olc::vd2d curr;
        for (double t = increment; t <= 1+increment; t+=increment) {
            curr = interpolateBezier(points, t);
            DrawLine((int) round(prev.x), (int) round(prev.y), (int) round(curr.x), (int) round(curr.y));
            prev = curr;
        }
    }
};

int main() {
    std::cout << "Starting program" << std::endl;
    Interpolation program;
    if (program.Construct(width, height, 1, 1, fullscreen))
        program.Start();

    return 0;
}