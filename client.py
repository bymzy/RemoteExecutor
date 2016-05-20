#! /bin/python
# -*- coding: utf-8 -*-

import sys
import socket

def exec_command(command):
	HOST = "192.168.2.18"
	PORT = 8889
	BUFFER = 1024*1024*4

	try:
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		s.connect((HOST, PORT))
		s.sendall(command)
		while True:
			data = s.recv(BUFFER)
			if not data:
				break;
			print data,
	except socket.error as e:
		print str(e)

	s.close()

if __name__ == "__main__":
	exec_command("ls /root")


