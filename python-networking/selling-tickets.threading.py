#! /usr/bin/python3
from threading import Lock
import socketserver
import argparse
import random
import time

BUFSIZE = 1024

class MyTCPSocketHandler(socketserver.BaseRequestHandler):
    def handle(self):
        global ticket_count
        lock.acquire()
        delay = 1
        self.request.sendall(f"Preparing a ticket, please wait for {delay} seconds\n".encode())
        time.sleep(delay)
        self.request.sendall(f"Your ticket number is {ticket_count}\n".encode())
        ticket_count += 1
        lock.release()
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("port", type=int, help="The port number the server will listen on")
    args = parser.parse_args()

    ticket_count = 1
    lock = Lock()


    server = socketserver.ThreadingTCPServer(("", args.port), MyTCPSocketHandler)

    server.serve_forever()