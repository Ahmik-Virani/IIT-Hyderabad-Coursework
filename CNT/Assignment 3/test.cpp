#include<bits/stdc++.h>
using namespace std;
#define ll long long

int main(){
    mt19937_64 state(time(nullptr));
    ll r;
    ll p = 5;
    int t = 100;
    while(t--){
        r = state() % p;
        cout << r << '\n';
    }
}