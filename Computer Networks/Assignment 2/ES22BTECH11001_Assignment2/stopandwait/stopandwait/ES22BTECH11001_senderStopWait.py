import socket
from PIL import Image
import base64
import time
import os

senderIP = "10.0.0.1"
senderPort   = 20001
recieverAddressPort = ("10.0.0.2", 20002)
bufferSize  = 1024 #Message Buffer Size

# Create a UDP socket at reciever side
socket_udp = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)


while True:

    # Send to server using created UDP socket
    msg = input("Please input name of file to be sent: ")	
    #msg = "testFile.jpg"
    file_size = os.path.getsize(msg)
    file = open(msg, "rb")
    
    no_of_chunks = int((file_size + 1024 - 1) / 1024)

    chunk = 0
    desired_ack = '0'
    no_of_retransmissions = 0
    first_send = True
   # start_time = time.time()
    #end_time = time.time()
    while chunk < no_of_chunks:
        sequence_number = chunk % 2
        sequence_byte = sequence_number.to_bytes(2, byteorder='big')
        is_last = 0
        if chunk == no_of_chunks - 1:
            is_last = 1
	
        last_bit = is_last.to_bytes(1, byteorder='big')
 
        byte = file.read(1024)
        message=sequence_byte+last_bit+byte

        timer = 0.100	# in seconds

        while True:
            if first_send == True and chunk == 0:
                start_time  = time.time()
                first_send = False
            socket_udp.settimeout(timer)
            try:
                socket_udp.sendto(message, recieverAddressPort)
                msgFromServer = socket_udp.recvfrom(bufferSize)
                ack = msgFromServer[0].decode()
                print(ack)
                if ack == ( 'ACK' + desired_ack):
                    if chunk == no_of_chunks - 1:
                        end_time = time.time()
                    if desired_ack == '0':
                        desired_ack = '1'
                    else:
                        desired_ack = '0'
                    break
                else:
                    no_of_retransmissions = no_of_retransmissions + 1
                    print('Out of order ACK recieved, retransmitting')
            except socket.timeout:
                no_of_retransmissions = no_of_retransmissions + 1
                print('No ACK recieved, retransmitting')

        chunk = chunk + 1

    print("Total nuber of retransmission: ", no_of_retransmissions)
    file_size_kb = file_size / 1024
    execution_time = end_time - start_time
    print("Throughput: ", file_size_kb / execution_time)
    break