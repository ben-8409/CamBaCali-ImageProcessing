#ifndef JSONGENERATOR_H
#define JSONGENERATOR_H

//STL
#include <map>
#include <string>
#include <opencv2/core/core.hpp>


class JsonGenerator
{
public:
    JsonGenerator();
    std::string generateResults(const std::map<std::string, cv::Scalar> screens,
                                const std::string localPath, const std::string uuid,
                                int lap);
    std::string masksReadyMessage(const std::string uuid);
};

#endif // JSONGENERATOR_H
