#! /usr/bin/python3
import socketserver
import argparse
import time

BUFSIZE = 1024

class MyTCPSocketHandler(socketserver.BaseRequestHandler):
    def handle(self):
        self.data = self.request.recv(BUFSIZE).strip()
        delay = len(self.data)
        print(f"{self.client_address} waiting for {delay} seconds")
        time.sleep(delay)
        print(f"{self.client_address} answered")
        self.request.sendall(self.data.upper())

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("port", type=int, help="The port number the server will listen on")
    args = parser.parse_args()
    
    server = socketserver.ForkingTCPServer(("", args.port), MyTCPSocketHandler)

    server.serve_forever()