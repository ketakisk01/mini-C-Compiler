brew int score = 85;
brew float ratio = 0.75;
brew string title = "Results";
brew bool passed = hot;
brew char grade = 'A';

if (score >= 90) {
    grade = 'S';
} else {
    grade = 'A';
}

while (score > 80) {
    brew int bonus = 2;
    score = score - bonus;
}

serve(title);
serve(grade);
serve(passed);

return score;
