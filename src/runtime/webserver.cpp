#include "webserver.h"

namespace dex {

WebServer::WebServer(int p) : port(p) {}

void WebServer::route(const std::string& path, const std::string& method, const std::string& handlerName) {
    std::cout << "Route registered: " << method << " " << path << " -> " << handlerName << std::endl;
}

void WebServer::start() {
    std::cout << "Starting server on port " << port << std::endl;
}

} // namespace dex
