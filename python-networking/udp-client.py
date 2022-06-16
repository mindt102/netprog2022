#! /usr/bin/python3
import socket
import argparse

def main(host, port, message):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    try:
        s.sendto(" ".join(message).encode(), (host, port))
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