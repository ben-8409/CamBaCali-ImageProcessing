#ifndef SCREENREGISTRATION_H
#define SCREENREGISTRATION_H

#include <string>
#include <map>
#include <vector>
#include <opencv2/core/core.hpp>
#include "colordetector.hpp"

class ScreenRegistration
{
private:
    std::map<std::string,cv::Mat> masks;
    std::map<std::string,cv::Scalar> meanColors;
    cv::Mat display_mask;
    cv::Mat erosionElement;
    cv::Mat dilationElement;
    ColorDetector colordetector;

public:
    ScreenRegistration();
    ScreenRegistration(const cv::Mat &mask, const float distance);
    void addScreen(const std::string id, const cv::Mat &highlight_image);
    std::map<std::string,cv::Scalar> calcMeanColors(const cv::Mat &sample_image);
    cv::Mat getMask(const std::string id);
    void setMask(const std::string id, const cv::Mat &mask);
    cv::Scalar getMeanColor(const std::string id, const cv::Mat &sample_image);
    cv::Mat applyMasks(const cv::Mat &sample_image);
    cv::Mat previewMeanColors(const cv::Mat &sample_image);
};

class screen_not_found
{
public:
    screen_not_found(std::string id)
        : screenId(id){}
    std::string what()
    {

        return message + screenId;
    }
private:
    std::string message = "Screen id not found: ";
    std::string screenId;
};
#endif // SCREENREGISTRATION_H
