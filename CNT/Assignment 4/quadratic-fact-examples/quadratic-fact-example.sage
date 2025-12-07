p=97
F=Integers(p)
R.<x>=F[]

a=2
f=x^2-a

h=1

while (h.degree()!=1):
	r=F.random_element()
	t=(p-1)/2
	h=gcd((x-r)^t-1,f))

print(h)
g=f/h
print(g)