import { createRequire } from "module";

class Server {
  #nativeServer;

  constructor(cb) {
    const NativeServer = this.#importNativeServer();
    this.#nativeServer = new NativeServer(cb);
  }

  listen(port) {
    this.#nativeServer.listen(8080);
  }

  #importNativeServer() {
    const require = createRequire(import.meta.url);
    const { Server } = require("../../build/Release/server");
    return Server;
  }
}

const server = new Server((msg) => console.log("cb called... msg: ", msg));
server.listen(8000);
