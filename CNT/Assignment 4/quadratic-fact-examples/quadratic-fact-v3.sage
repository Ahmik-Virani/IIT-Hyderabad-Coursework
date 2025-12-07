def polypowmod(g,k,f,p):
	F=Integers(p)
	R.<x>=F[]
	if (k==0):
		return R(1)
	if (k==1):
		return g
	h=polypowmod(g,k//2,f,p)
	h=(h^2)%f
	if (k%2==1):
		h=(h*g)%f
	return h

def quadraticFactors(f,p):
	F=Integers(p)
	R.<x>=F[]
	h=f
	while (h.degree()!=1): 
		r=F.random_element()
		t=int((p-1)/2)
		h=polypowmod((x-r),t,f,p)
		h=(h-1)%f
	h=gcd(h,f)
	g=f/h
	print(h)
	print(g)
	
	
def main():
	p=int(input("Enter a prime: "))
	if (not is_prime(p)):
		print("Not a prime.")
		return
	F=Integers(p)
	R.<x>=F[]	
	a=int(input("Enter the value of a: "))
	a=F(a)
	b=a^((p-1)/2)
	if (b==-1):
		print("Not a square")
		return	
	
	f=x^2-a
	quadraticFactors(f,p)
	
if __name__=="__main__":
	main()

