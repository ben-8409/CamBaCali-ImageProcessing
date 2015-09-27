#include "utils.hpp"

#include <iostream>
#include <Poco/File.h>

Utils::Utils(){

};

bool Utils::existsAndExecutable(const Poco::Path path) {
  Poco::File pathF(path);
  if(pathF.exists() && pathF.canExecute()) {
    return true;
  } else {
    std::cout << "Input dir does not exist or is not accessible" << std::endl;
    return false;
  }
};
