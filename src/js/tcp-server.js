import { createRequire } from "module";

class TcpServer {
  #nativeTcpServer;

  constructor(cb, errorCb) {
    const NativeTcpServer = this.#importNativeTcpServer();
    this.#nativeTcpServer = new NativeTcpServer(cb, errorCb);
  }

  listen(port) {
    this.#nativeTcpServer.listen(port);
  }

  #importNativeTcpServer() {
    const require = createRequire(import.meta.url);
    const { TcpServer } = require("../../build/Release/tcp-server");
    // const { TcpServer } = require("../../build/Debug/tcp-server");
    return TcpServer;
  }
}

try {
  setTimeout(() => {
    const tcpServer = new TcpServer(
      (msg) => console.log("cb called... msg: ", msg),
      (err) => console.error("has error: ", err)
    );
    tcpServer.listen(8000);
  });
} catch (error) {
  console.log("error when starting server...");
  console.error(error);
}
