bool verifyMatrixMultiplication(
    const Matrix& A,
    const Matrix& B,
    const Matrix& claimed_C, // Prover's claim
    int prime
) {
    int n = A.size();
    FieldElement::p = prime;

    // --- Step 1: Verifier picks random challenges r1, r2 ---
    std::vector<FieldElement> r1 = {FieldElement(rand() % prime, prime)}; // Random x
    std::vector<FieldElement> r2 = {FieldElement(rand() % prime, prime)}; // Random z

    // --- Step 2: Prover sends claimed evaluation v = f_C(r1, r2) ---
    FieldElement v = evaluateMLE(claimed_C, r1, r2);

    // --- Step 3: Define the sum-check polynomial g(y) = f_A(r1, y) * f_B(y, r2) ---
    auto g = [&](const std::vector<FieldElement>& y) {
        FieldElement f_A = evaluateMLE(A, r1, y);
        FieldElement f_B = evaluateMLE(B, y, r2);
        return f_A * f_B;
    };

    // --- Step 4: Verify sum_{y ∈ {0,1}^log n} g(y) == v using sum-check ---
    return sumcheck(g, /*num_variables=*/1, v); // 1 variable (y) for n=2
}