/* 
* Course Name : Computational Number Theory & Algebra
* Programming HW : 1
* Name : Ahmik Virani
* Roll Number : ES22BTECH11001
*/


// I have used the following documentation
// https://gmplib.org/manual/Integer-Functions

#include <iostream>
#include <string>
#include <gmp.h>
#include <fstream>
#include <sstream>

using namespace std;

void extendedEuclid(const char *A, const char *B){

    // GMP integers are stored in objects of type mpz_t
    mpz_t a, b;

    // Variables as used in the extended euclids algorithm
    mpz_t u, v, x, y;
    
    // We also need to store the previous values
    mpz_t prev_u, prev_v, prev_x, prev_y;

    // Define the remainder and the quotiend
    mpz_t r, q;

    // Initialize the values
    mpz_inits(a, b, u, v, x, y, prev_u, prev_v, prev_x, prev_y, r, q, NULL);

    // set the value of A or B into a or b respectively using base 10
    mpz_set_str(a, A, 10);
    mpz_set_str(b, B, 10);

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

    if(swapped){
        gmp_printf("x=%Zd,y=%Zd,c=%Zd\n", y, x, a);
    }else{
        gmp_printf("x=%Zd,y=%Zd,c=%Zd\n", x, y, a);
    }
}

int main() {

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
        // From each line, get the value of a and b as mentioned in the problem statement
        stringstream ss(line);
        string string_a, string_b;

        getline(ss, string_a, ',');
        getline(ss, string_b, ',');

        // We need to convert the strings to (char *) as per the documentation of gmp
        const char* a = string_a.c_str();
        const char* b = string_b.c_str();

        extendedEuclid(a, b);
    }

    // Close the file
    file.close();

    return 0;
}