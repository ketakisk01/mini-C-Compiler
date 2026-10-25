brew int x = 10;
brew int y = 5;
brew int sum = x + y;

serve(sum);

if (sum > 10) {
    serve("Sum is greater than 10");
} else {
    serve("Sum is 10 or less");
}

brew int i = 0;

while (i < 3) {
    serve(i);
    i = i + 1;
}

return 0;