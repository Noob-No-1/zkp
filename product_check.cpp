#include <iostream>
#include <vector>
#include <mcl/fp.hpp>
#include <cassert>
#include <mcl/bn.hpp>

using namespace mcl::bn;
using namespace std;
using namespace mcl;

// Define prime field
using Field = Fr;

// Set modulus (e.g. BN254 prime)
const char* FIELD_MODULUS = "21888242871839275222246405745257275088548364400416034343698204186575808495617";

// Evaluate g(r) = ∏_{i=1}^n (1 - r_i + r_i * x_i) at Boolean x
Field g_eval(const vector<Field>& r, const vector<int>& x) {
    Field res = 1;
    for (size_t i = 0; i < r.size(); ++i) {
        Field term;
        if (x[i] == 0) {
            term = 1 - r[i];
        } else {
            term = r[i];
        }
        res *= term;
    }
    return res;
}

// Example f(x): product of (1 + x_i)
Field f_eval(const vector<int>& x) {
    Field prod = 1;
    for (int xi : x) {
        prod *= (1 + xi);
    }
    return prod;
}

// ∑_{x∈{0,1}^n} f(x) * g(r, x)
Field product_check_sum(const vector<Field>& r, int n) {
    Field sum = 0;
    int total = 1 << n;
    for (int i = 0; i < total; ++i) {
        vector<int> x(n);
        for (int j = 0; j < n; ++j) {
            x[j] = (i >> j) & 1;
        }
        Field f = f_eval(x);
        Field g = g_eval(r, x);
        sum += f * g;
    }
    return sum;
}

int main() {
    // Setup field
    initPairing();

    int n = 1;
    vector<Field> r(n);
    for (int i = 0; i < n; ++i) {
        r[i].setByCSPRNG();
    }

    Field result = product_check_sum(r, n);

    cout << "Product check result over {0,1}^" << n << ": " << result.getStr() << endl;
    cout << "r[0]: " << r[0].getStr() << endl;
    cout << "Expected: " << (r[0] + Field(1)).getStr() << endl;

    return 0;
}