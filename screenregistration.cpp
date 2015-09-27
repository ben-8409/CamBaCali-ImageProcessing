#include "screenregistration.hpp"
#include <iostream>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

ScreenRegistration::ScreenRegistration()
{
  dilationElement = getStructuringElement(MORPH_RECT, cv::Size(3, 3));
  erosionElement = getStructuringElement(MORPH_RECT, cv::Size(12, 12));
}

ScreenRegistration::ScreenRegistration(const cv::Mat &mask, const float distance)
{
    mask.copyTo(display_mask);
    colordetector.setTargetColor(0, 255, 0);
    colordetector.setDistance(distance);
}

void ScreenRegistration::addScreen(const std::string id, const cv::Mat &highlight_image)
{
    cv::Mat image;
    highlight_image.copyTo(image, display_mask);

    cv::Mat mask;
    mask = colordetector.binaryMask(image);

    //erode the mask to remove small areas
    erode(mask, mask, erosionElement);
    //dilate to fill small holes
    dilate(mask, mask, dilationElement);

    mask.copyTo(masks[id]);

    std::cout << "Added screen mask for " << id << std::endl << std::flush;
}

cv::Mat ScreenRegistration::getMask(const std::string id)
{
    if(masks.find(id)==masks.end())
        throw screen_not_found(id);
    return masks[id];
}


void ScreenRegistration::setMask(const std::string id, const cv::Mat &mask)
{
    mask.copyTo(masks[id]);

    std::cout << "Added screen mask for " << id << std::endl << std::flush;
}

std::map<std::string, cv::Scalar> ScreenRegistration::calcMeanColors(const cv::Mat &sample_image)
{
    //interator to loop through the map
    //typedef std::map<std::string, cv::Mat>::iterator it_type;

    for(const auto &screen : masks )
    {
        meanColors[screen.first] = getMeanColor(screen.first, sample_image);
    }

    return meanColors;
}

cv::Scalar ScreenRegistration::getMeanColor(const std::string id, const cv::Mat &sample_image)
{
    std::cout << "Trying to get screen mask for " << id << std::endl << std::flush;
    if(masks.find(id)==masks.end())
        throw screen_not_found(id);

    return cv::mean(sample_image, masks[id]);
}

cv::Mat ScreenRegistration::applyMasks(const cv::Mat &sample_image)
{
  cv::Mat output;
  output.create(sample_image.rows, sample_image.cols, sample_image.type());
  for(const auto &screen : masks )
  {
    std::cout << "Copy region of screen " << screen.first << std::endl;
    std::cout << "Image Size" << sample_image.size() << " Mask size " << screen.second.size() << std::endl;
    sample_image.copyTo(output, screen.second);
  }
  return output;
}

cv::Mat ScreenRegistration::previewMeanColors(const cv::Mat &sample_image)
{
  cv::Mat output;
  output.create(sample_image.rows, sample_image.cols, sample_image.type());
  for(const auto &screen : masks )
  {
    cv::Scalar meanColor = getMeanColor(screen.first, sample_image);
    output.setTo(meanColor, screen.second);
  }

  return output;
}
