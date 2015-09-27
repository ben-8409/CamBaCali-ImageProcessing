#include "colordetector.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

ColorDetector::ColorDetector()
{
    this->maxDist = 70.0;//TODO: Warum dieser Wert?
    //this->targetColor[0] = 87.737;
    //this->targetColor[1] = -86.185;
    //this->targetColor[2] = 83.131;
    this->setTargetColor(0, 255, 0);
}


void ColorDetector::setTargetColor(unsigned char R, unsigned char G, unsigned char B)
{
    //temporary 1pixel image to convert color spaces
    cv::Mat rgb, cielab;
    rgb.create(1, 1, CV_8UC3);
    cielab.create(1, 1, CV_32FC3);

    rgb.at<cv::Vec3b>(0,0)[0] = B;
    rgb.at<cv::Vec3b>(0,0)[1] = G;
    rgb.at<cv::Vec3b>(0,0)[2] = R;

    rgb.convertTo(cielab, CV_32FC3, 1.0/255.0);
    std::cout << "Scaled: B " << cielab.at<cv::Vec3f>(0,0)[0] <<
                 " G " << cielab.at<cv::Vec3f>(0,0)[1] <<
                 " R " << cielab.at<cv::Vec3f>(0,0)[2] << std::endl;

    cv::cvtColor(cielab, cielab, CV_BGR2Lab);

    targetColor = cielab.at<cv::Vec3f>(0,0);
    std::cout<< "Target color set to L* " << targetColor[0]
             << ", a " << targetColor[1]
             << ", b " << targetColor[2] << std::endl;
}

void ColorDetector::setDistance(float d)
{
    this->maxDist = d;
}

double ColorDetector::getDistance(const cv::Vec3f color)
{
    //return abs(color[0] - targetColor[0]) + abs(color[1] - targetColor[1]) + abs(color[2] - targetColor[2]);
    //double L = color[0] - targetColor[0];
    double a = color[1] - targetColor[1];
    double b = color[2] - targetColor[2];
    //double distance = cv::sqrt(L*L + a*a + b*b);
    double distance = cv::sqrt(a*a + b*b);
    return distance;
    //return static_cast<int>(cv::norm(targetColor,color));
}

cv::Mat ColorDetector::binaryMask(const cv::Mat &image)
{
    //create matrix for the binary image
    result.create(image.rows, image.cols, CV_8U);
    std::cout << "Created mask image" << std::endl;

    //create matrix for converted image
    cielab.create(image.rows, image.cols, CV_32FC3);
    std::cout << "Created cielab image" << std::endl;
    //convert values from uint to float
    //see: http://stackoverflow.com/questions/11386556/converting-an-opencv-bgr-8-bit-image-to-cie-lab
    image.convertTo(cielab, CV_32FC3, 1.0/255.0);
    std::cout << "Converted image to 32bit float" << std::endl;
    cv::cvtColor(cielab, cielab, CV_BGR2Lab);
    std::cout << "Converted image to cielab" << std::endl;

    cv::Mat_<cv::Vec3f>::const_iterator it = cielab.begin<cv::Vec3f>();
    cv::Mat_<cv::Vec3f>::const_iterator it_end = cielab.end<cv::Vec3f>();
    cv::Mat_<uchar>::iterator itout = result.begin<uchar>();

    //image processing loop.
    for ( ; it != it_end; ++it, ++itout) {
        if(getDistance(*it) < maxDist) {
            *itout = 255;
        } else {
            *itout = 0;
        }
    }

    return result;
}
