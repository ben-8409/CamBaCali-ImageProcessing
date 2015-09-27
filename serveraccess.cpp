#include "serveraccess.hpp"
#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include <iostream>
#include <Poco/URI.h>
#include <Poco/Path.h>
#include <Poco/StreamCopier.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPMessage.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>

ServerAccess::ServerAccess(std::string &address)
{
    server_address = address;
}

std::vector <std::string> ServerAccess::getScreenIds()
{
    std::vector<std::string> screenIds;
    Poco::URI uri(server_address + "/api/screens");
    //std::string url = server_address + "/api/screens";
    Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());

    //prepare path
    std::string path(uri.getPath());

    //prepare and send request
    Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1);
    session.sendRequest(req);

    //get response
    Poco::Net::HTTPResponse res;

    std::cout << res.getStatus() << res.getReason() << std::endl;

    std::istream &is = session.receiveResponse(res);
    std::string response_body;
    Poco::StreamCopier::copyToString(is, response_body);

    //Parsing
    rapidjson::Document d;
    d.Parse(response_body.c_str());

    assert(d.IsObject());

    assert(d.HasMember("screens"));
    {
        const rapidjson::Value& screens = d["screens"];
        assert(screens.IsArray());
        for (rapidjson::Value::ConstValueIterator itr = screens.Begin(); itr != screens.End(); ++itr) {
            screenIds.push_back(itr->GetString());
        }
    }

    return screenIds;
}

void ServerAccess::postResults(std::string uuid, std::string results)
{
    std::string apiPath;
    if(uuid!="") {
      apiPath = "/api/calibrations/" + uuid + "/measurement";
    } else {
      apiPath = "/api/calibrations/measurement";
    }
    Poco::URI uri(server_address + apiPath);
    //std::string url = server_address + "/api/screens";
    Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());

    //prepare path
    std::string path(uri.getPath());
    //prepare and send request
    std::string reqBody(results);
    Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_POST, path, Poco::Net::HTTPMessage::HTTP_1_1);
    req.setContentType("application/json");
    req.setContentLength(reqBody.length());
    req.setKeepAlive(true);
    std::ostream& oustr = session.sendRequest(req);
    //oustr << results;
    oustr << reqBody;
    req.write(std::cout);

    //get response
    Poco::Net::HTTPResponse res;

    std::cout << res.getStatus() << res.getReason() << std::endl;

    std::istream &is = session.receiveResponse(res);
    Poco::StreamCopier::copyStream(is, std::cout);
}

void ServerAccess::masksReady(std::string uuid, std::string message)
{
    Poco::URI uri(server_address + "/api/calibrations/" + uuid + "/masksReady");
    //std::string url = server_address + "/api/screens";
    Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());

    //prepare path
    std::string path(uri.getPath());
    //prepare and send request
    std::string reqBody(message);
    Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_POST, path, Poco::Net::HTTPMessage::HTTP_1_1);
    req.setContentType("application/json");
    req.setContentLength(reqBody.length());
    req.setKeepAlive(true);
    std::ostream& oustr = session.sendRequest(req);
    //oustr << results;
    oustr << reqBody;
    req.write(std::cout);

    //get response
    Poco::Net::HTTPResponse res;

    std::cout << res.getStatus() << res.getReason() << std::endl;

    std::istream &is = session.receiveResponse(res);
    Poco::StreamCopier::copyStream(is, std::cout);
}
