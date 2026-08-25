# Mini C Compiler

A lightweight Mini C Compiler built from scratch in C++ for college Compiler Design projects. It implements a complete compiler front-end up to **Syntax Analysis (Parsing)**.

## Goal & Architecture

The compiler takes raw C-like source code and processes it through the following front-end pipeline:

```text
Source Code
   ↓
Lexical Analyzer (Lexer) -> Identifies Tokens, logs Lexical Errors
   ↓
Token Stream
   ↓
Syntax Analyzer (Parser) -> Enforces Grammar, builds Scoped Symbol Table
   ↓
Abstract Syntax Tree (AST) -> Visualized in human-readable tree form
```

## Supported Mini C Features

- **Data Types**: `int`, `float`, `char`, `bool`
- **Variables & Declarations**: Block-scoped variable declarations with optional assignment initializers.
- **Functions**: Function declarations (like `int main() { ... }`) with parameter handling.
- **Assignments**: Identifier updates (`x = expression;`).
- **Operators**:
  - Arithmetic: `+`, `-`, `*`, `/`, `%`
  - Relational: `<`, `>`, `<=`, `>=`, `==`, `!=`
  - Logical: `&&`, `||`, `!`
  - Unary / Inc / Dec: Prefix/Postfix `++` and `--`
- **Control Flow**: `if-else` conditionals and `while` loop iterations.
- **Built-ins**: `print(expression);` statement.
- **Functions**: `return` expressions.
- **Comments**: Single-line (`//`) and multi-line (`/* ... */`) comments.
- **Delimiters**: Parentheses `()`, Braces `{}`, semicolons `;`, commas `,`.

---

## Grammar Specification

The compiler parses expressions using a left-recursion-free grammar to ensure compatibility with LL-style recursive descent. Operator precedence is handled via cascading levels (lowest to highest):

1. **Assignment**: `=`
2. **Logical OR**: `||`
3. **Logical AND**: `&&`
4. **Equality**: `==`, `!=`
5. **Relational**: `<`, `>`, `<=`, `>=`
6. **Additive**: `+`, `-`
7. **Multiplicative**: `*`, `/`, `%`
8. **Unary**: Prefix `!`, `-`, `++`, `--`
9. **Postfix**: Postfix `++`, `--`
10. **Primary**: Literals (Integer, Float, Char, Bool, String), Identifiers, and parenthesized expressions `( expr )`

---

## Project Structure

```text
MiniCCompiler/
├── src/
│   ├── main.cpp          # CLI Compiler Driver
│   ├── Token.h           # Token types & structs
│   ├── Lexer.h           # Lexer class definition
│   ├── Lexer.cpp         # Lexical scanner logic
│   ├── AST.h             # AST node structures
│   ├── Parser.h          # Recursive Descent Parser definition
│   ├── Parser.cpp        # Syntax analyzer logic
│   ├── SymbolTable.h     # Scoped Symbol Table interface
│   └── SymbolTable.cpp   # Scope tracking & lookup logic
├── examples/
│   ├── valid_program.mc  # Syntactically valid Mini C code
│   ├── lexical_error.mc  # Program with malformed literals/comments
│   └── syntax_error.mc   # Program with unbalanced brackets/missing semicolons
└── README.md             # Project documentation (this file)
```

---

## How to Build and Run

### Prerequisites
A modern C++ compiler supporting C++17 or later (e.g. `g++`, `clang++`, or MSVC `cl`).

### Compilation
Use a terminal/shell to compile all source files from the project root:

```bash
g++ -std=c++17 -o MiniCCompiler src/main.cpp src/Lexer.cpp src/Parser.cpp src/SymbolTable.cpp
```

### Execution
Run the compiled binary on a Mini C source file (`.mc`):

```bash
./MiniCCompiler examples/valid_program.mc
```

---

## Example Compiler Output

When running on `examples/valid_program.mc`, the compiler outputs the following:

### 1. Lexical Analysis (Tokens Table)
Prints each scanned token with its type, source lexeme, line, and column position:
```text
----------------------------------------------------------------------
Token Type     Lexeme                   Line           Column         
----------------------------------------------------------------------
KEYWORD        int                      1              1              
IDENTIFIER     main                     1              5              
DELIMITER      (                        1              9              
DELIMITER      )                        1              10             
...
```

### 2. Scoped Symbol Table
Logs all variables and functions declared throughout compilation:
```text
----------------------------------------------------------------------
Identifier Name     Data Type           Decl Line      Decl Column    
----------------------------------------------------------------------
a                   int                 2              9              
b                   int                 3              9              
c                   int                 4              9              
main                int function        1              5              
...
```

### 3. Syntax Analysis Status
Prints either `Success` or lists `Lexical Errors` / `Syntax & Semantic Errors` (along with line/column context and token info):
```text
Success: Parsing completed with 0 errors.
```

### 4. Abstract Syntax Tree (AST) Visualization
Prints the parsed syntax tree with indentation levels displaying construct hierarchies:
```text
Program
  Function (int main) Params: none
    Block
      Declaration (int a)
        Initializer:
          Literal (INTEGER): 10
      Declaration (int c)
        Initializer:
          BinaryExpression (+)
            Identifier: a
            BinaryExpression (*)
              Identifier: b
              Literal (INTEGER): 2
```
