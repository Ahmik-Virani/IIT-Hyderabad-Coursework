import socket

recieverIP = "10.0.0.2"
recieverPort   = 20002
bufferSize  = 2048 #Message Buffer Size

# bytesToSend = str.encode(msgFromServer)

# Create a UDP socket
socket_udp = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

# Bind socket to localIP and localPort
socket_udp.bind((recieverIP, recieverPort))

print("UDP socket created successfully....." )

file = open("my_file", "wb")

desired_seq_no = 0
ack_no = '0'


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

    if seq_no != desired_seq_no:
        if ack_no == '1':
            message = str.encode("ACK" + '0')
            print('Out of order packet, sending dublicate ACK')
            socket_udp.sendto(message, senderAddress)
        else:
            message = str.encode("ACK" + '1')
            print('Out of order packet, sending dublicate ACK')
            socket_udp.sendto(message, senderAddress)
        continue
    

    #print them just for understanding
    msgString = "Message from Client:{}".format(recievedMessage)
    detailString  = "Client IP Address:{}".format(senderAddress)
    #print(msgString)
    #print(detailString)

    # Sending a reply to client
    message = str.encode("ACK" + ack_no)
    print("ACK" + ack_no)
    socket_udp.sendto(message, senderAddress)

    if ack_no == '1':
        ack_no = '0'
    else:
        ack_no = '1'
    
    desired_seq_no = (desired_seq_no + 1) % 2
    file.write(payload)

    if is_last==1:
        for _ in range(10):
            socket_udp.sendto(message, senderAddress)
        break

file.close()