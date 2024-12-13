import socket

#WRITE CODE HERE:
#1. Create a KEY-VALUE pairs (Create a dictionary OR Maintain a text file for KEY-VALUES).

# Create a text file if it is not already present
file_obj = open('storage.txt', 'a')
file_obj.close()

#dst_ip = str(input("Enter Server IP: "))
dst_ip = "10.0.1.2"

s = socket.socket()
print ('Socket successfully created')

dport = 12346

s.bind((dst_ip, dport))
print ('socket binded to %s' %(dport))

s.listen(5)
print ("socket is listening")

while True:
  c, addr = s.accept()
  print ('Got connection from ', addr )
  recvmsg = c.recv(1024).decode()
  response = "dummy"
  
  #Write your code here
  #1. Uncomment c.send 
  #2. Parse the received HTTP request
  
  #The format we wish to recieve requests is that the key and value is specified in the URL itself, so any body would be just ignored in our case
  #First let us remove any additional body
  request_line = recvmsg.split('\n')[0]
  method = request_line.split(' ')[0]

  #3. Do the necessary operation depending upon whether it is GET, PUT or DELETE

  if method == "GET":
    key = request_line.split(' ')[1].split('=')[1]
    version = request_line.split(' ')[2]

    #Next let us check the file for the key
    flag = False    #Flag to indicate if we have found the key or not
    value = None
    with open("storage.txt", 'r') as file_obj:
      for line in file_obj:
        k, v = line.strip().split()
        if k == key:
          value = v
          flag = True
          break
    
    if flag:
      response = version + ' 200 OK \n' + '{key: ' + key + ', value: ' + value + '}'
    else:
      response = version + ' 404 Not Found'
  
  elif method == "DELETE":
    key = request_line.split(' ')[1].split('/')[2]
    version = request_line.split(' ')[2]

    #Next let us check the file for the key
    flag = False    #Flag to indicate if we have found the key or not
    value = None

    # Read all lines into memory
    with open("storage.txt", 'r') as file_obj:
      lines = file_obj.readlines()

    flag = False
    new_line = []

    for line in lines:
      k, v = line.strip().split()
      if k == key:
        flag = True
        value = v
      else:
        new_line.append(line)
    
    if flag:
      #Write back the updated file
      with open("storage.txt", 'w') as file_obj:
        file_obj.writelines(new_line)
      response = version + ' 200 OK \n' + 'Deleted the following : {key: ' + key + ', value: ' + value + '}'
    else:
      #No changes made
      response = version + ' 404 Not Found\nNothing to be deleted'

  elif method == "PUT":
    key = request_line.split(' ')[1].split('/')[2]
    value = request_line.split(' ')[1].split('/')[3]
    version = request_line.split(' ')[2]

    #Next let us check the file for the key
    flag = False    #Flag to indicate if we have found the key or not

    # Read all lines into memory
    with open("storage.txt", 'r') as file_obj:
      lines = file_obj.readlines()

    flag = False
    new_line = []

    for line in lines:
      k, v = line.strip().split()
      if k == key:
        changed_line = key + ' ' + value + '\n'
        new_line.append(changed_line)
        flag = True
      else:
        new_line.append(line)
    
    #Note that new_line contains all the lines which must be stored, so to that, we must also add the new key value pair
    if not flag:
      key_val_pair = key + ' ' + value + '\n'
      new_line.append(key_val_pair)

    #Now write this updated version to the file
    with open("storage.txt", 'w') as file_obj:
      file_obj.writelines(new_line)    

    #If the flag is found, which means that the key already exists, then we must update the key
    if flag:
      response = version + ' 200 OK \n' + 'Updated the following : {' + key + ':' + value + '}' 
    else:
      response = version + ' 200 OK \n' + 'Inserted the following : {' + key + ':' + value + '}' 
  
  #4. Send response
  response = response + '\r' + '\n'
  c.send(response.encode())
  ##################

  c.close()
  #break