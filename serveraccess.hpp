#ifndef SERVERACCESS_H
#define SERVERACCESS_H

#include <string>
#include <vector>

class ServerAccess
{
private:
    std::string server_address;
public:
    ServerAccess(std::string &address);
    std::vector <std::string> getScreenIds();
    void postResults(std::string uuid, std::string results);
    void masksReady(std::string uuid, std::string message);
};

#endif // SERVERACCESS_H
