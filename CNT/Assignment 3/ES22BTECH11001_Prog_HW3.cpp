/*
* Name : Ahmik Virani 
* Roll Number : ES22BTECH11001
* Course : Computational Number Theory
* Programming HW 3
*/

#include <iostream>
#include <string>
#include <gmp.h>
#include <fstream>
#include <sstream>
#include <random>
#include <ctime>

#define ll long long

using namespace std;

// This part of the code is same as the first homework assignment
void extendedEuclid(mpz_t A, mpz_t B, mpz_t x, mpz_t y, mpz_t g){

    // declare the local a and b values
    mpz_t a, b;

    // Variables as used in the extended euclids algorithm
    mpz_t u, v;
    
    // We also need to store the previous values
    mpz_t prev_u, prev_v, prev_x, prev_y;

    // Define the remainder and the quotiend
    mpz_t r, q;

    // Initialize the values
    mpz_inits(a, b, u, v, x, g, y, prev_u, prev_v, prev_x, prev_y, r, q, NULL);

    // Initialize the values of a and b to A and B
    mpz_add_ui(a, A, 0);
    mpz_add_ui(b, B, 0);

    // Initialize the values of x, y, u, v
    mpz_set_str(x, "1", 10);
    mpz_set_str(y, "0", 10);
    mpz_set_str(u, "0", 10);
    mpz_set_str(v, "1", 10);


    // variable to track if a and b are swapped
    bool swapped = false;

    // Compare the values of a and b
    // If b more than a, then swap them
    if(mpz_cmp(a, b) < 0){
        mpz_swap(a, b);
        swapped = true;
    }

    // Run the loop while b != 0
    while(mpz_sgn(b) != 0){
        // Store the previous value of x, y, u, v as we need them for computing the new values of the same
        mpz_set(prev_x, x);
        mpz_set(prev_y, y);
        mpz_set(prev_u, u);
        mpz_set(prev_v, v);

        // r <- a mod b
        mpz_mod(r, a, b);   

        // q <- floor(a/b)
        mpz_fdiv_q(q, a, b);

        // a <- b
        mpz_set(a, b);

        // b <- r
        mpz_set(b, r);

        // x <- prev_u
        mpz_set(x, prev_u);
        
        // y <- prev_v
        mpz_set(y, prev_v);

        // u <- prev_x - q * prev_u
        mpz_mul(u, q, prev_u);
        mpz_sub(u, prev_x, u);

        // v <- y - prev_q * prev_v
        mpz_mul(v, q, prev_v);
        mpz_sub(v, prev_y, v);
    }

    // Ensure that we handle if a < b originally
    if(swapped){
        mpz_add_ui(g, a, 0);
        mpz_add_ui(x, y, 0);
        mpz_add_ui(y, x, 0);
    }else{
        mpz_add_ui(g, a, 0);
        mpz_add_ui(x, x, 0);
        mpz_add_ui(y, y, 0);
    }
}

// Function to convert a number to binary
string int_to_binary(mpz_t x){
    string binary = "";
    mpz_t r;
    mpz_inits(r, NULL);
    while(mpz_sgn(x)){
        mpz_mod_ui(r,x,2);
        if(mpz_cmp_ui(r,0) == 0){
            binary += '0';
        }else{
            binary += '1';
        }
        mpz_fdiv_q_ui(x, x, 2);
    }
    return binary;
}

// Code used from Prog-HW2
ll exponentiation(ll N, ll A, ll B){

    mpz_t n, a, b, answer;
    mpz_inits(n, a, b, answer, NULL);
    mpz_set_ui(n, N);
    mpz_set_ui(a, A);
    mpz_set_ui(b, B);

    // If b < 0 then we need to compute a^-1
    // Thus to do that we need to ensure gcd = 1
    // If so, then the value of a^-1 = x

    // Here we declare the variables gcd, x, y 
    // These will be needed only if b < 0
    mpz_t g, x, y;

    // If b < 0, then we compuet a^-1
    if(mpz_sgn(b)==-1){
        // need to replace a to a^-1
        extendedEuclid(a, n, x, y, g);

        // Check for unique inverse
        if(mpz_cmp_si(g,1) == 0){
            // make b positive
            mpz_mul_si(b,b,-1);

            // the value of a^-1 will be x
            mpz_add_ui(a,x,0);

            // ensure that it belongs to mod n
            mpz_mod(a, a, n);
        }else{
            // return answer = -1 if it does not exist
            return -1;
        }
    }

    // We need to write b in its binary form
    string bin_b = int_to_binary(b);

    unsigned long int bits_of_b = bin_b.size();

    // For binary exponentiation we compute powers of a
    // Current values is a dummy variable to store these powers
    mpz_t current_value;
    mpz_inits(current_value, NULL);

    mpz_add(current_value, current_value, a);
    mpz_add_ui(answer, answer, 1);

    for(int i = 0 ; i < bits_of_b ; i++){
        if(bin_b[i] == '1'){
            // answer = (answer * current_value) % n;
            mpz_mul(answer, answer, current_value);
            mpz_mod(answer, answer, n);
        }

        // current_value = (current_value * current_value) % n;
        mpz_mul(current_value, current_value, current_value);
        mpz_mod(current_value, current_value, n);
    }

    return mpz_get_ui(answer);
}

void Tonelli_Shanks(ll a, ll p){

    // Check for p = 2
    if(p==2){
        if(a==1){
            cout << 1 << '\n';
        }else{
            cout << -1 << '\n';
        }
        return ;
    }

    // for p > 2
    // check that a ^ ((p-1)/2) = 1
    if(exponentiation(p, a, (p-1)/2) == 1){
        ll m = p-1;

        ll r, l;

        // p-1 = 2^l * m
        while(m % 2 == 0){
            m/=2;
            l++;
        }

        // b = a^m
        ll b = exponentiation(p, a, m);

        // Need to solve y^2 = b
        vector<ll> B;
        ll exponent = 1;
        while(1){
            // exponent stores 2^j
            // we need value of b^(2^j)
            B.push_back(exponentiation(p, b, exponent));
            if(*B.rend() == 1){
                break;
            }
            exponent *= 2;
        }

        if(B.size() - 1 == l){
            printf("-1\n");
        }else{
            // x <- a ^ (m+1)/2
            ll x = exponentiation(p, a, (m+1)/2);

            if(B.size()==1){
                cout << x << '\n';
            }else{
                // generate random r till r ^ (p-1)/2 == -1
                mt19937_64 state(time(nullptr));
                ll r;
                while(1){
                    r = state() % p;

                    // need to check if r^((p-1)/2) == -1
                    if(exponentiation(p, r, (p-1)/2) == p-1){
                        break;
                    }
                }
                // s = r^m, e=2^l
                ll s = exponentiation(p, r, m), e = (1<<l);

                for(int i = B.size() - 1 ; i > 0 ; i--){
                    // if B[i-1] == s^(e/2)
                    if(B[i-1] == exponentiation(p, s, e/2)){
                        e = e/2;
                    }else{
                        e = e/2 + (p-1)/2;
                    }
                }

                // Y <- s ^ (p-1-e/2)
                ll Y = exponentiation(p, s, p - 1 - e/2); 

                // return xY
                // since x*Y can be larger than long long, store in GMP format
                mpz_t large_answer, large_x, large_Y;
                mpz_inits(large_answer, large_x, large_Y, NULL);
                mpz_set_ui(large_x, x);
                mpz_set_ui(large_Y, Y);
                mpz_mul(large_answer, large_x, large_Y);
                mpz_mod_ui(large_answer, large_answer, p);

                ll answer = mpz_get_ui(large_answer);
                cout << min(answer, p - answer) << '\n';
            }
        }
    }else{
        printf("-1\n");
    }
}

int main(){
    // Get the filename as an input from the user
    string filename;
    cout << "Enter the input file name: ";
    cin >> filename;

    ifstream file(filename);

    // Print an error message if file does not exist
    if(!file.is_open()){
        cout << "ERROR: Not able to open file! Please check if the file exists.\n";
        return 1;
    }

    // Go through all the lines in the csv file
    string line;
    while(getline(file, line)){
        // From each line, get the value of a and p as mentioned in the problem statement
        stringstream ss(line);
        string string_a, string_p;

        getline(ss, string_a, ',');
        getline(ss, string_p, ',');

        // we need to convert them into long long format
        ll a = stoll(string_a);
        ll p = stoll(string_p);
        Tonelli_Shanks(a, p);
    }

    // Close the file
    file.close();

    return 0;
}