#include <mcl/bn.hpp>
#include <iostream>
#include <vector>
#include <cassert>

using namespace mcl::bn;
using namespace std;

using Field = Fr;

// Returns LHS and RHS product for permutation check
pair<Field, Field> permutation_check(const vector<Field>& a, const vector<Field>& b, const Field& beta, const Field& gamma) {
    assert(a.size() == b.size());
    size_t n = a.size();

    Field lhs = 1;
    Field rhs = 1;

    for (size_t i = 0; i < n; ++i) {
        Field i_field; i_field.setStr(to_string(i), 10);

        Field a_term = a[i] + beta * i_field + gamma;
        Field b_term = b[(i + 1) % n] + beta * i_field + gamma;

        lhs *= a_term;
        rhs *= b_term;
    }

    return {lhs, rhs};
}

int main() {
    initPairing(); // BN254 prime field

    // Example: a = [4,2,1,3], b = [3,4,2,1] (a rotated right)
    vector<Field> a(4), b(4);
    a[0].setStr("4", 10); a[1].setStr("2", 10); a[2].setStr("1", 10); a[3].setStr("3", 10);
    b[0].setStr("3", 10); b[1].setStr("4", 10); b[2].setStr("2", 10); b[3].setStr("1", 10);

    Field beta, gamma;
    beta.setByCSPRNG();
    gamma.setByCSPRNG();

    auto [lhs, rhs] = permutation_check(a, b, beta, gamma);

    cout << "Permutation check:" << endl;
    cout << "LHS: " << lhs.getStr() << endl;
    cout << "RHS: " << rhs.getStr() << endl;
    cout << ((lhs == rhs) ? "✅ Passed" : "❌ Failed") << endl;

    return 0;
}