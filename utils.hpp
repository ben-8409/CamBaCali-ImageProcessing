#ifndef UTILS_H
#define UTILS_H

#include <Poco/Path.h>

using namespace Poco;
//STL
class Utils
{
public:
  Utils();
    bool existsAndExecutable(const Path path);
};

#endif // UTILS_H
