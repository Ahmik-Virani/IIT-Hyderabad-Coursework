import socket

recieverIP = "10.0.0.2"
recieverPort   = 20002
bufferSize  = 2048  #Message Buffer Size

# bytesToSend = str.encode(msgFromServer)

# Create a UDP socket
socket_udp = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

# Bind socket to localIP and localPort
socket_udp.bind((recieverIP, recieverPort))

print("UDP socket created successfully....." )

file = open("my_file", "wb")

last_received_seq_no = -1
ack_no = 0


while True:

    #wait to recieve message from the server
    bytesAddressPair = socket_udp.recvfrom(bufferSize)

    #split the recieved tuple into variables
    recievedMessage = bytesAddressPair[0]
    senderAddress = bytesAddressPair[1]

    header = recievedMessage[:3]
    payload = recievedMessage[3:]

    

    is_last = int.from_bytes(header[2:], byteorder='big')
    seq_no = int.from_bytes(header[:2], byteorder='big')
    print(seq_no)

    if seq_no != ((last_received_seq_no + 1) % (1<<16)) and last_received_seq_no != -1:
        ack_no = last_received_seq_no.to_bytes(2, byteorder='big')
        #print('Out of order packet, sending dublicate ACK')
        socket_udp.sendto(ack_no, senderAddress)
        continue
    elif seq_no != ((last_received_seq_no + 1) % (1<<16)) and last_received_seq_no == -1:
        continue    

    #print them just for understanding
    msgString = "Message from Client:{}".format(recievedMessage)
    detailString  = "Client IP Address:{}".format(senderAddress)
    #print(msgString)
    #print(detailString)

    last_received_seq_no = seq_no

    # Sending a reply to client
    ack_no = last_received_seq_no.to_bytes(2, byteorder='big')
    #print(ack_no)
    socket_udp.sendto(ack_no, senderAddress)
    
    file.write(payload)
    
    if is_last==1:
        break

file.close()
