#! /usr/bin/python3
import socket
import argparse

BUFSIZE = 1024

def main(host, port, message):
    s = socket.socket()
    s.connect((host, port))
    try:
        s.send(" ".join(message).encode())
        msg = s.recv(BUFSIZE)
        print(msg.decode())
    except Exception as exc:
        print(f"Error sending: {exc}")
        s.close()
        exit(-1)
    s.close()

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("host", type=str, help="The ip or domain name of the server")
    parser.add_argument("port", type=int, help="The port that server is listening on")
    parser.add_argument("message", type=str, nargs="+",help="The message to send")
    args = parser.parse_args()

    main(args.host, args.port, args.message)