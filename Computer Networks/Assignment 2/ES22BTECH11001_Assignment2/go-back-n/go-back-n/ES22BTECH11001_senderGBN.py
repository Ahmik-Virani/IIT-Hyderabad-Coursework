import socket
from PIL import Image
import time
import os

senderIP = "10.0.0.1"
senderPort   = 20001
recieverAddressPort = ("10.0.0.2", 20002)

window_size = 32


bufferSize  = 1024 #Message Buffer Size

window_begin, window_end, no_of_chunks = 0, 0, 0

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
    desired_ack = 0
    no_of_retransmissions = 0
    first_send = True

    to_send_packets = []
 

    timer = 0.015	# in seconds

    times = [time.time()] * no_of_chunks


    while chunk < no_of_chunks:
        to_send_packets.append(file.read(1024))
        chunk += 1

    window_begin = 0
    window_end = window_begin + window_size - 1

    first_send = True
    start_time = time.time()
    end_time = time.time()

    chunk = 0
    window_begin = 0
    sent_last_packet = False
    last_seq_no = (no_of_chunks - 1) % (1<<16)
    done = False
    while True:
        # This is either the beginning or a timeout
        # Send all the packets in the window
        window_end = min(window_begin + window_size - 1, no_of_chunks - 1)
        for i in range(window_begin, window_end + 1, 1):
            sequence_number = i % (1<<16)
            sequence_byte = sequence_number.to_bytes(2, byteorder='big')
            is_last = 0
            if i == no_of_chunks - 1:
                is_last = 1
                sent_last_packet = True

            last_bit = is_last.to_bytes(1, byteorder='big')
            byte = to_send_packets[i]
            message=sequence_byte+last_bit+byte
            times[i] = time.time()
            socket_udp.sendto(message, recieverAddressPort)
            if first_send == True:
                first_send = False
                start_time = time.time()

        while True: 
            if window_begin >= no_of_chunks:
                end_time = time.time()
                done = True
                break
            time_left = (times[window_begin] + timer) - time.time()
            socket_udp.settimeout(max(time_left, 0))
    

            try:
                msgFromServer = socket_udp.recvfrom(bufferSize)
                #Next wait for an ack
                ack_no = int.from_bytes(msgFromServer[0], byteorder='big')
                if sent_last_packet == True and ack_no == last_seq_no:
                    end_time = time.time()
                    done = True
                    break
                while ack_no >= window_begin:
                    if window_end < no_of_chunks - 1:
                        # Send the next sequence number
                        sequence_number = (window_end + 1) % (1<<16)
                        sequence_byte = sequence_number.to_bytes(2, byteorder='big')
                        is_last = 0
                        if window_end + 1 == no_of_chunks - 1:
                            is_last = 1
                        last_bit = is_last.to_bytes(1, byteorder='big')
                    
                        byte = to_send_packets[window_end+1]
                        message=sequence_byte+last_bit+byte
                        times[window_end + 1] = time.time()
                        socket_udp.sendto(message, recieverAddressPort)
                        #print("hi")

                        window_begin = window_begin + 1
                        window_end = window_end + 1
                    else:
                        window_begin = window_begin + 1

                if done:
                    break

            except socket.timeout:
                # If timeout, resend all the packets
                break

            except BlockingIOError:
                continue

        if done == True:
            break

    # print("Total nuber of retransmission: ", no_of_retransmissions)
    file_size_kb = file_size / 1024
    execution_time = end_time - start_time
    print("Throughput: ", file_size_kb / execution_time)
    #print(no_of_chunks)
    break