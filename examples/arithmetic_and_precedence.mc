// Arithmetic and Operator Precedence Test
brew a = 10;
brew b = 5;
brew c = 2;

// Tests precedence of * and / over + and -
brew res1 = a + b * c;       // 10 + 10 = 20
brew res2 = (a + b) * c;     // 15 * 2 = 30
brew res3 = a / b + c;       // 2 + 2 = 4
brew res4 = a % 3;           // 1

serve(res1);
serve(res2);
serve(res3);
serve(res4);
