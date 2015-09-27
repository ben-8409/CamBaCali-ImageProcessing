#include "jsongenerator.hpp"

//STL
#include <iostream>
//Rapidjson
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

using namespace rapidjson;

JsonGenerator::JsonGenerator()
{
}

std::string JsonGenerator::generateResults(const std::map<std::string, cv::Scalar> screens,
                                           const std::string localPath, const std::string uuid,
                                           int lap)
{
    StringBuffer s_buf;
    Writer<StringBuffer> writer(s_buf);

    writer.StartObject();
    writer.String("dataType");
    writer.String("RGB_INT");
//    writer.String("localPath");
//    writer.String(localPath.c_str());
//    if(uuid!="")
//    {
//      writer.String("uuid");
//      writer.String(uuid.c_str());
//    }
//    writer.String("lap");
//    writer.Int(lap);

    writer.String("screens");
    writer.StartObject();
    for(auto &screen : screens)
    {
        writer.String(screen.first.c_str());
        writer.StartArray();
        writer.Int(screen.second[2]);
        writer.Int(screen.second[1]);
        writer.Int(screen.second[0]);
        //opencv uses bgr, i want rgb
        //writer.StartArray();
        //writer.Double(screen.second[2]);
        //writer.Double(screen.second[1]);
        //writer.Double(screen.second[0]);
        writer.EndArray();
    }
    writer.EndObject();

    writer.EndObject();
    //end results object

    return s_buf.GetString();
}

std::string JsonGenerator::masksReadyMessage(const std::string uuid)
{
  StringBuffer s_buf;
  Writer<StringBuffer> writer(s_buf);

  writer.StartObject();
  writer.String("uuid");
  writer.String(uuid.c_str());
  writer.EndObject();

  return s_buf.GetString();
}
