brew marks = 78;
brew attendance = 85;

if (marks >= 40) {
    serve("Pass");
} else {
    serve("Fail");
}

while (attendance > 80) {
    serve("Good Attendance");
    attendance = attendance - 5;
}

serve("Program Finished");