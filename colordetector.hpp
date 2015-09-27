#ifndef COLORDETECTOR_H
#define COLORDETECTOR_H

#include <opencv2/core/core.hpp>

class ColorDetector
{

private:
    double maxDist;
    cv::Vec3f targetColor;
    cv::Mat cielab;
    cv::Mat result;
    double getDistance(const cv::Vec3f color);

public:
    ColorDetector();
    cv::Mat binaryMask(const cv::Mat &image);
    void setTargetColor(unsigned char R, unsigned char G, unsigned char B);
    void setDistance(float d);
};

#endif // COLORDETECTOR_H
