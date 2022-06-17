#! /usr/bin/python3
import socketserver
import argparse
import random
import time
import fcntl

BUFSIZE = 1024
TICKET_FILENAME = "ticket_count.txt"

class MyTCPSocketHandler(socketserver.BaseRequestHandler):
    def handle(self):
        self.request.sendall(f"Preparing a ticket, please wait.\n".encode())
        try:
            f = open(TICKET_FILENAME, "r+")
            ticket_count = int(f.read())
            fcntl.flock(f, fcntl.LOCK_EX)
        except (IOError, OSError, ValueError) as e:
            print(f"Error: {e}")
            self.request.sendall("500 - Internal Server Error")
            return

        try:
            self.request.sendall(f"Your ticket number is {ticket_count}\n".encode())
            f.seek(0)
            f.write(str(ticket_count+1))
            f.truncate()
        finally:
            fcntl.flock(f, fcntl.LOCK_UN)
            f.close()

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("port", type=int, help="The port number the server will listen on")
    args = parser.parse_args()

    ticket_count = 1
    with open(TICKET_FILENAME, "w") as f:
        f.write(str(ticket_count))

    print(f"Selling tickets on port {args.port}")
    server = socketserver.ForkingTCPServer(("", args.port), MyTCPSocketHandler)

    server.serve_forever()