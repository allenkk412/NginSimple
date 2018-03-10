#python2.7.5
#coding=utf-8

import socket

if __name__ == "__main__":
    sockfd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sockfd.connect(('localhost', 5000))
    message = ""
    while 1:
        message = raw_input("Please input:")
        sockfd.send(message)
        message = sockfd.recv(5000)
        print message
    sockfd.close()
