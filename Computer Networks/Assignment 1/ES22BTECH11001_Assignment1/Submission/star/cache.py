import socket

#WRITE CODE HERE:
#1. Create a KEY-VALUE pairs (Create a dictionary OR Maintain a text file for KEY-VALUES).

# Create a text file if it is not already present
file_obj = open('cache.txt', 'a')
file_obj.close()

#dst_ip = str(input("Enter Server IP: "))
dst_ip = "10.0.1.2"

s = socket.socket()
print ("Socket successfully created")

dport = 12346

s.bind((dst_ip, dport))
print ("socket binded to %s" %(dport))

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
    mark = False    #Mark that the value is updated in the server
    with open("cache.txt", 'r') as file_obj:
      lines = file_obj.readlines()

    for line in lines:
      k, v, is_updated = line.strip().split()
      if k == key and is_updated == 'no':
        value = v
        flag = True
        break
      elif k == key and is_updated == 'yes':
        mark = True
        break
    
    #If latest key found in cache, return from here itself
    if flag:
      response = version + ' 200 OK \n' + '{key: ' + key + ', value: ' + value + '}' + '\nLocation : Cache (h2)'

    #If latest key not found in cache, request the origin server
    else:
      s_server = socket.socket()
      serverIP = "10.0.1.3"
      serverPort = 12346
      s_server.connect((serverIP, serverPort))
      s_server.send(recvmsg)
      response = s_server.recv(1024).decode()

      status_code = response.split('\n')[0].split(' ')[1]

      if status_code == '200':
        value = response.split('\n')[1].split(':')[2].split(' ')[1].split('}')[0]
     
        #If key not updated
        if mark:
          with open("cache.txt", 'r') as file_obj:
            lines = file_obj.readlines()

          flag = False
          new_line = []

          for line in lines:
            k, v, is_updated = line.strip().split()
            if k == key:
              changed_line = key + ' ' + value + ' no\n'
              new_line.append(changed_line)
              flag = True
            else:
              new_line.append(line)
          
          with open("cache.txt", 'w') as file_obj:
	    file_obj.writelines(new_line)
               
        else:
          with open("cache.txt", 'a') as file_obj:
            to_write = key + ' ' + value + ' no\n'
            file_obj.write(to_write)
  
  elif method == "DELETE":
    key = request_line.split(' ')[1].split('/')[2]
    version = request_line.split(' ')[2]

    #First check if the server has the key to be deleted
    s_server = socket.socket()
    serverIP = "10.0.1.3"
    serverPort = 12346
    s_server.connect((serverIP, serverPort))
    s_server.send(recvmsg)
    response = s_server.recv(1024).decode()

    #If server has, only then check the cache
    if response.split(" ")[1] == "200":
      #Next let us check the file for the key
      flag = False    #Flag to indicate if we have found the key or not
      value = None

      # Read all lines into memory
      with open("cache.txt", 'r') as file_obj:
        lines = file_obj.readlines()

      flag = False
      new_line = []

      for line in lines:
        k, v, is_updated = line.strip().split()
        if k == key:
          flag = True
   	  value = v
        else:
          new_line.append(line)
    
      #If the object to be deleted is present in the cache, then delete it
      if flag:
        #Write back the updated file
        with open("cache.txt", 'w') as file_obj:
          file_obj.writelines(new_line)

  elif method == "PUT":
    s_server = socket.socket()
    serverIP = "10.0.1.3"
    serverPort = 12346
    s_server.connect((serverIP, serverPort))
    s_server.send(recvmsg)
    response = s_server.recv(1024).decode()

    check_key = request_line.split(' ')[1].split('/')[2]

    with open("cache.txt", 'r') as file_obj:
      lines = file_obj.readlines()
    
    flag = False
    new_line = []

    for line in lines:
      k, v, is_updated = line.strip().split()
      if k == check_key:
        changed_line = line.split(' ')[0] + ' ' + line.split(' ')[1] + ' yes\n'
        new_line.append(changed_line)
        flag = True
      else:
        new_line.append(line)

    with open("cache.txt", 'w') as file_obj:
      file_obj.writelines(new_line)

  
  #4. Send response
  response = response + '\r' + '\n'
  c.send(response.encode())
  ##################

  c.close()
  #break