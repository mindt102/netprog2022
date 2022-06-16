#! /usr/bin/python3
import socketserver
import argparse
import random
import time

BUFSIZE = 1024

class MyTCPSocketHandler(socketserver.BaseRequestHandler):
    def handle(self):
        with open("ticket_count.txt", "r") as f:
            ticket_count = int(f.read())
        with open("ticket_count.txt", "w") as f:
            f.write(str(ticket_count+1))
        delay = random.randint(1,5)
        self.request.sendall(f"Preparing a ticket, please wait for {delay} seconds\n".encode())
        time.sleep(delay)
        self.request.sendall(f"Your ticket number is {ticket_count}\n".encode())

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("port", type=int, help="The port number the server will listen on")
    args = parser.parse_args()

    ticket_count = 1
    with open("ticket_count.txt", "w") as f:
        f.write(str(ticket_count))

    server = socketserver.ForkingTCPServer(("", args.port), MyTCPSocketHandler)

    server.serve_forever()