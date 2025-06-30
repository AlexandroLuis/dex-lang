server = WebServer(8080)

server.route("/", "GET", func() {
    return "Hello, Dex!"
})

server.start()
