#ifndef DEX_WEBSERVER_H
#define DEX_WEBSERVER_H

#include <string>
#include <iostream>

namespace dex {

class WebServer {
public:
    WebServer(int port);
    void route(const std::string& path, const std::string& method, const std::string& handlerName);
    void start();
private:
    int port;
};

} // namespace dex

#endif // DEX_WEBSERVER_H
