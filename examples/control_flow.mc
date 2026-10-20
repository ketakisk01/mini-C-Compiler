// Control Flow Test: Conditionals, Relational Operators, and Loops
brew score = 85;
brew status = 0;

if (score >= 90) {
    status = 1;
    serve("Grade A");
} else {
    if (score >= 80) {
        status = 2;
        serve("Grade B");
    } else {
        status = 3;
        serve("Grade C");
    }
}

brew count = 1;
brew sum = 0;

while (count <= 5) {
    sum = sum + count;
    count = count + 1;
}

serve("Loop Sum 1 to 5:");
serve(sum);
