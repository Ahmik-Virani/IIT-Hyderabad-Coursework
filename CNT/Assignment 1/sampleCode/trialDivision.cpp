#include<iostream>
#include<vector>
#include<gmpxx.h>

using namespace std;
int main()
{
	mpz_class a,b;
	mpz_t n;
	vector<mpz_class> divisorList{};


    //cout<<"Enter the number to be factorized: ";
    //mpz_inp_str(n,stdin,10);

    mpz_ui_pow_ui(n,2,64);
    mpz_class m(n);

    m=m+1;

    a=2;
	b=a*a;
    while(m>b)
    {
        while (m%a==0)
        {
			mpz_class d(a);
		    divisorList.push_back(d);
            m=m/a;
        }
        a=a+1;
		b=a*a;
    }

    if (m>1)
    {
		mpz_class d(m);
		divisorList.push_back(d);
    }

	cout<<"\n The prime factors of "<<n<<" are: ";
	 for (auto i = divisorList.begin(); i != divisorList.end(); i++)
        cout<<" "<<*i;
	cout<<endl;


    return 0;
}
