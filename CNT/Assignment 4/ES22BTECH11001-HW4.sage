# Computational Number Theory Programming HW 4
# Name : Ahmik Virani
# Roll Number : ES22BTECH11001

import csv

# Function to pick a random polynomial of degree < deg(f)
def random_poly(f):
    g = R(0)
    # Ensure that g does not equal 0
    while g == R(0):
        for i in range(f.degree()):
            g += F.random_element()*x^i
    return g

# Algorithm 8 from lecture notes
def IrreducibleFactors(f, i):
    # Ensure we do not go into any infinite loop
    if f.degree() <= 0:
        return []

    if(f.degree()==i):
        return [f]
    
    h1 = R(1)
    h2 = R(1)
    while(h1==R(1) or h2==R(1)):
        # Pick a random polynomial of degree less than that of f
        g = random_poly(f)
        exp = ((p**i) - 1) // 2
        g_power_exp = pow(g, exp, f)
        h1 = gcd(f, g_power_exp - 1)
        h2 = f//h1
    List1 = IrreducibleFactors(h1, i)
    List2 = IrreducibleFactors(h2, i)
    
    # This forms a union because all the factors have power 1, thus no factors repeat
    return List1 + List2

# Assuming file name is input-CZ.csv
with open("input-CZ.csv", newline='') as csvfile:
    reader = csv.reader(csvfile)

    first_line = next(reader)
    p = int(first_line[0])
    F=Integers(p)
    R.<x>=F[]

    for row in reader:
        f = R(0)

        d = int(row[0])
        for coeff in range(1, d+2, 1):
            f += F(int(row[coeff]))*x^(d-coeff+1)

        # Distinct Degree factorization
        # Algorithm 7 in lecture notes
        i=1
        g=f
        factors=[]
        exponents=[]
        while (g!=R(1)):
            x_power_p_power_i = pow(x, p**i, g)
            fi = gcd(g, x_power_p_power_i - x)
            # If there are no factors, no need to append
            if fi != R(1):
                factors.append(fi)
                exponents.append(i)
                g = g//fi
            i = i + 1

        answer=[]
        for (fx, ei) in zip(factors, exponents):
            answers_i = IrreducibleFactors(fx, ei)
            for element in answers_i:
                answer.append(element)
        
        for i in range(len(answer)):
            if i > 0:
                print(' * ', sep='', end='')
            print('(', answer[i], ')',sep='', end='')
            
        # Extra line for readability
        print()
        print()