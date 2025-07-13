#define MCLBN_FP_UNIT_SIZE 4            // 4 x 64‑bit limbs  ⇒ BN‑254
#define MCLBN_FR_UNIT_SIZE MCLBN_FP_UNIT_SIZE   // keep Fr in the same field

#include <mcl/bn.hpp> //complie with -I flag
#include <vector>
#include <iostream>

namespace bn = mcl::bn;
using Fr = bn::Fr;

inline Fr getRandomFr() {
    Fr r;
    r.setByCSPRNG();
    return r;
}

inline Fr fr_pow(Fr x, uint64_t exp) {
    Fr r(1);
    while (exp) {
        if (exp & 1) {
            r *= x;
        }
        x *= x;
        exp >>= 1;
    }
    return r;
}

struct Monom {
    Fr coeff;
    std::vector<int> exps;
};

struct MPoly {
    std::vector<Monom> monoms;
    int n;
    Fr eval(const std::vector<Fr>& x) const {
        Fr sum(0);
        for (const auto& m : monoms) {
            Fr t = m.coeff;
            for (size_t i = 0; i < n; ++i) {
                t *= fr_pow(x[i], m.exps[i]);
            }
            sum += t;
        }
        return sum;
    }
};

struct UPoly {
    std::vector<Fr> coeffs;
    int deg() const { return coeffs.size() - 1; }
    Fr eval(Fr x) const {
        Fr sum(0);
        for (size_t i = 0; i < coeffs.size(); ++i) {
            sum += coeffs[i] * fr_pow(x, i);
        }
        return sum;
    }
    UPoly& operator+=(const UPoly& rhs) {
        if (rhs.deg() > deg()) {
            coeffs.resize(rhs.deg() + 1);
        }
        for (size_t i = 0; i < rhs.coeffs.size(); ++i) {
            coeffs[i] += rhs.coeffs[i];
        }
        return *this;
    }
};

UPoly make_slice(const MPoly& f, const std::vector<Fr>& p) {
    int j  = p.size();
    int rem = f.n - j - 1;
    int N = 1 << rem;
    UPoly acc{{Fr(0)}};
    for (int mask =0; mask < N; ++mask) {
        UPoly cur{{Fr(0)}};
        for (const auto& m : f.monoms) {
            int degX = m.exps[j];
            Fr coeff = m.coeff;
            //computing r1 to rj-1 stuff, inefficient, but for illustration purpose i throw it here
            for (int i = 0; i < j && !coeff.isZero(); ++i) {
                if (m.exps[i]){
                    coeff *= fr_pow(p[i], m.exps[i]);
                }
            }
            //computing bj+1 to bn stuff which needs enumeration, ie tail factor
            for (int i = 0; i < rem && !coeff.isZero(); ++i) {
                int deg = j+1+i;
                int exp = m.exps[deg];
                if (exp && ((mask>>i) & 1) == 0) {
                    coeff = Fr(0);
                }
            }
            if (coeff.isZero()) continue;
            if ((int)cur.coeffs.size() <= degX) {
                cur.coeffs.resize(degX + 1, Fr(0));
            }
            cur.coeffs[degX] += coeff;
        }
        acc += cur;
    }
    return acc;
}

int main() {
    bn::initPairing();
    MPoly f; //the true function 
    f.n = 3;
    f.monoms = {
        {Fr(3), {0,0,0}},
        {Fr(5), {1,0,0}},
        {Fr(7), {0,1,0}},
        {Fr(11), {0,0,1}},
    };
    MPoly g; //3-var polynomial demo of a honest prover
    g.n = 3;
    g.monoms = {
        {Fr(3), {0,0,0}},
        {Fr(5), {1,0,0}},
        {Fr(8), {0,1,0}},
        {Fr(11), {0,0,1}},
    };
    std::vector<Fr> p;
    UPoly p_slice = make_slice(g, p);

    std::vector<int> deg(g.n);
    for (auto& m: g.monoms) {
        for (int i=0; i < g.n; ++i) {
            deg[i] = std::max(deg[i], m.exps[i]);
        }
    }
    bool ok = true;
    std::vector<Fr> ver_r;
    UPoly cur = p_slice;

    Fr C1 = cur.eval(Fr(0))+ cur.eval(Fr(1));
    if (cur.deg() > deg[0]) ok = false; //failed degree check
    if (C1 != cur.eval(Fr(0)) + cur.eval(Fr(1))) ok = false; //failed evaluation check
    //i know it looks weird lmao, but i am writing this to follow the protocol procedure
    Fr r1 = getRandomFr();
    ver_r.push_back(r1); p.push_back(r1);
    Fr rhs = cur.eval(r1);

    for (int i = 1; i < g.n && ok; ++i) {
        cur = make_slice(g, p);
        Fr Ci = cur.eval(Fr(0))+ cur.eval(Fr(1));
        if (cur.deg() > deg[i]) ok = false; //failed degree check
        if (Ci != rhs) ok = false; //failed evaluation check 
        Fr ri = getRandomFr();
        ver_r.push_back(ri); p.push_back(ri);
        rhs = cur.eval(ri);
    }

    Fr final_eval = f.eval(ver_r);

    if(rhs != final_eval) ok = false;

    if (ok) {
        std::cout << "Verification passed!" << std::endl;
    } else {
        std::cout << "Verification failed!" << std::endl;
    }
}
//compile using: g++ -std=c++17 -O2 \
  -I"/Users/zhaozhihao/GitHub/mcl/include" \
  "/Users/zhaozhihao/GitHub/zkp/sumcheck/multi-sumcheck.cpp" \
  "/Users/zhaozhihao/GitHub/mcl/build/lib/libmcl.a" \
  -o multi-sumcheck
//run using: ./multi-sumcheck