// Optimizations Test: Constant Folding, Propagation, Algebraic Simplification
brew x = 10 + 20;     // Folded to 30
brew y = x * 2;       // Constant propagated (30 * 2) -> folded to 60
brew z = y + 0;       // Algebraic simplification: + 0 removed
brew w = z * 1;       // Algebraic simplification: * 1 removed
brew zero = w * 0;    // Algebraic simplification: * 0 -> 0

serve(x);
serve(y);
serve(z);
serve(w);
serve(zero);
