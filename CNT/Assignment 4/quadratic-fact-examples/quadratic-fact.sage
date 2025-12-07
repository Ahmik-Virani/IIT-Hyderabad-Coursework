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

	h=f
	while (h.degree()!=1): 
		r=F.random_element()
		t=(p-1)/2
		h=gcd((x-r)^t-1,f)
	print(h)
	g=f/h
	print(g)
	
	
if __name__=="__main__":
	main()


	