int main() {
    int a = 10;
    int b = 20;
    int c = a + b * 2;
    float d = 3.14;
    bool isGreater = c > 30;
    char grade = 'A';

    // Conditional branches
    if (isGreater && c != 50) {
        print(c);
    } else {
        print(0);
    }

    // While loop execution
    while (a > 0) {
        print(a);
        a--;
    }

    return 0;
}
