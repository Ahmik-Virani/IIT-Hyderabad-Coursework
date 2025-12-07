#n=int(input("Enter the number to be factorized: "))

n=2**64+1
m=n

factorList=[1]
d=2
while (m>d*d):
	while (m%d==0):
		factorList.append(d)
		m=m/d
	d=d+1
if (m>1):
	factorList.append(m)
print "The prime factors of ",n," are: ",(factorList)

