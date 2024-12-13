import socket

serverIP = "10.0.1.2"

dst_ip = str(input("Enter dstIP: "))
#dst_ip = serverIP
s = socket.socket()

request = str(input("Enter the request"))

#You could alternatively alter the keys and values here
#request = "PUT /assignment/key10/val10 HTTP/1.1"
#request = "GET /assignment1?request=key10 HTTP/1.1"
#request = "DELETE /assignment1/key10 HTTP/1.1"

print(dst_ip)

port = 12346

s.connect((dst_ip, port))

#Write your code here:
#1. Add code to send HTTP GET / PUT / DELETE request. The request should also include KEY.
#2. Add the code to parse the response you get from the server.
s.send(request.encode())
print (s.recv(1024).decode())

s.close()
