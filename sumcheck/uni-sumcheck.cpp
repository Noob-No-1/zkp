#include <mcl/bn_c256.h>
#include <vector>
#include <iostream>
/*std::tuple<bool, Fr, std::vector<Fr>> Int_Sumcheck_fx(const std::vector<Fr>& f, const Fr& ClaimedValue, const unsigned& kVars){
    // P claims ClaimedValue
    Fr Claimed_Sum = ClaimedValue;
    // P computes 2 points and sends them to V
    int max_degree = 1;
    bool ok = false;
    std::vector<Fr> points(max_degree+1,Fr(0));
    std::vector<Fr> r_Challenges(kVars,Fr(0));
    std::vector<Fr> Sumcheck_f = f;
    for(unsigned int i=0; i<kVars; i++){
        // P computes max_degree+1 points and sends them to V
        std::vector<std::vector<Fr>> Auxiliary_f(max_degree+1, std::vector<Fr>(1<<(kVars-1-i),Fr(0)));
        std::vector<std::vector<Fr>> result(max_degree+1, std::vector<Fr>(1<<(kVars-1-i),Fr(1)));
        std::vector<Fr> result_sum(max_degree+1, Fr(0));
        for (unsigned kk=0;kk<(1u<<(kVars-1-i));kk++){
            Auxiliary_f[0][kk] = Sumcheck_f[kk*2];
            Auxiliary_f[1][kk] = Sumcheck_f[kk*2+1];
        }
        for(unsigned int j=0; j<max_degree+1; j++){
            for(unsigned int jj=0; jj<(1u<<(kVars-1-i)); jj++){
                result_sum[j] = result_sum[j] + Auxiliary_f[j][jj];
            }
        }
        // V checks 2 points sum = claimed_sum
        ok = (result_sum[0] + result_sum[1] == Claimed_Sum);
        if (!ok) break;
        // V generates random r_i and sends it to P. V computes next Claimed_Sum.
        Fr r_i;r_i.setRand();
        r_Challenges[i] = r_i;
        Claimed_Sum = Int_lagrangeInterpolation(result_sum,r_i,max_degree);
        // P maintains bkp table
        Sumcheck_f = Int_ri_Table_Compute_fx(kVars-i, Sumcheck_f, r_i);
    }
    return std::make_tuple(ok, Sumcheck_f[0], r_Challenges);
}*/