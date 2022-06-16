#! /usr/bin/python3
# -*- coding: utf-8 -*-
import socket
import argparse
import logging
BUFSIZE = 1024

FORMAT = '[%(asctime)s] client %(clientip)-15s sent %(message)s'
logging.basicConfig(level=logging.INFO, format=FORMAT)
def main(port):
    try:
        s = socket.socket()
        s.bind(("", port))
        s.listen(0)
        print(f"Started server on TCP/{port}")
    except Exception:
        logging.exception("Error binding")
        exit(-1)

    while True:
        try:
            cnx, addr = s.accept()
            d = {
                "clientip": addr[0],
            }
            msg = cnx.recv(BUFSIZE)
            logging.info(msg.decode(), extra=d)
            cnx.send("Message received!".encode())
        except Exception as e:
            logging.exception("Error receiving")
            s.close()
            exit(-1)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("port", type=int, help="The port number the server will listen on")
    args = parser.parse_args()
    main(args.port)


