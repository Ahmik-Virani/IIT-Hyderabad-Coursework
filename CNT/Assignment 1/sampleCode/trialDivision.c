#include<stdio.h>
#include<gmp.h>
#include<assert.h>

int main()
{
    mpz_t n,m,a,b;

    mpz_inits(n,m,a,b,NULL);

    //gmp_printf("Enter the number to be factorized: ");
    //mpz_inp_str(n,stdin,10);

    mpz_ui_pow_ui(n,2,64);
    mpz_add_ui(n,n,1);

    mpz_set(m,n);

    gmp_printf("\n The prime divisors of %Zd are: \n",n);
    mpz_set_ui(a,2);
	mpz_mul(b,a,a);
    while(mpz_cmp(m,b)>0)
    {
        while (mpz_divisible_p(m,a))
        {
            gmp_printf("\n %Zd",a);
            mpz_div(m,m,a);
        }
        mpz_add_ui(a,a,1);
		mpz_mul(b,a,a);
    }

    if (mpz_cmp_ui(m,1))
    {
        gmp_printf("\n %Zd",m);
    }

    mpz_clears(n,m,a,b,NULL);

    return 0;
}
