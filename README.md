# Mini C Compiler

A complete end-to-end Mini C Compiler built from scratch in C++. It implements the entire compiler pipeline from lexical analysis through syntax parsing, intermediate code generation (TAC), multi-pass code optimization, and target assembly language generation for x86 (IA-32).

## Architecture & Full Compiler Pipeline

```text
Source Code (.mc)
       ↓
[Phase 1] Lexical Analyzer (Lexer) -> Token Stream + Lexical Error Reporting
       ↓
[Phase 2] Syntax Analyzer (Parser) -> Recursive-Descent AST Construction
       ↓
[Phase 3] Semantic Analyzer (SemanticAnalyzer) -> Type Checking, Scope Validation, Semantic Error Reporting
       ↓
[Phase 4] Symbol Table -> Identifier, Scope, and Line Tracking
       ↓
[Phase 5] Intermediate Code Generator (IR) -> Three-Address Code (TAC)
       ↓
[Phase 6] Code Optimizer -> Constant Folding, Propagation, Algebraic Simplification, DCE
       ↓
[Phase 7] Assembly Code Generator -> Target x86 (IA-32 GAS) Assembly (.s)
       ↓
Target Assembler (GCC / MinGW) -> Executable (.exe)
```

---

## Supported Mini C Language Features

- **Variable Declarations**: `brew [type] <id> [= <expr>];` or `brew <id> [= <expr>];`
- **Data Types**: `int`, `float`, `char`, `bool` (`hot` = true, `cold` = false), `string`
- **Assignments**: `<id> = <expr>;`
- **Arithmetic Operators**: `+`, `-`, `*`, `/`, `%`
- **Relational Operators**: `<`, `>`, `<=`, `>=`, `==`, `!=`
- **Logical Operators**: `&&`, `||`, `!`
- **Unary Operators**: `-` (negation), `!`, `++`, `--`
- **Control Flow**:
  - Conditionals: `if (<cond>) <stmt> [else <stmt>]`
  - Loops: `while (<cond>) <stmt>`
- **Built-in I/O**:
  - `serve(<expr>);` (prints numeric values, strings, or expressions followed by newline)
  - `sip(<id>);` (reads integer input into variable)
  - `return [<expr>];` (function exit)
- **Blocks & Comments**: `{ ... }`, single-line `//`, multi-line `/* ... */`

---

## Compiler Phases

### Phase 1: Lexical Analysis (`Lexer.h`, `Lexer.cpp`, `Token.h`)
- Scans source code into strongly-typed `Token` objects.
- Tracks source position (line number and column) for informative error messages.
- Identifies keywords, identifiers, integer/float/char/string/boolean literals, operators, and delimiters.

### Phase 2: Syntax Analysis (`Parser.h`, `Parser.cpp`, `AST.h`)
- Implements recursive descent parsing.
- Handles operator precedence via cascading grammar rules.
- Builds an AST (`ASTNode` hierarchy) and parse tree for debugging.
- Reports syntax errors and aborts when invalid code is encountered.

### Phase 3: Semantic Analysis (`SemanticAnalyzer.h`, `SemanticAnalyzer.cpp`)
- Validates declarations, duplicate names, type mismatches, and undeclared symbols.
- Checks assignment compatibility and boolean/numeric conditions in `if`/`while`.
- Stops compilation when semantic errors are found.

### Phase 4: Symbol Table (`SymbolTable.h`, `SymbolTable.cpp`)
- Tracks declared variables, kinds, and declaration lines.
- Maintains scope-aware symbol lookup for semantic validation.

### Phase 5: Intermediate Code Generation (`TAC.h`, `TAC.cpp`, `IRGenerator.h`, `IRGenerator.cpp`)
- Translates AST into linear Three-Address Code (TAC).
- Produces arithmetic, comparison, branch, and I/O instructions used by the optimizer/code generator.

### Phase 6: Code Optimization (`Optimizer.h`, `Optimizer.cpp`)
- Performs constant folding, constant propagation, algebraic simplification, and dead-code elimination on TAC.
- Produces optimized intermediate code before final assembly emission.

### Phase 7: Target Assembly Generation (`CodeGenerator.h`, `CodeGenerator.cpp`)
- Generates x86 (IA-32) GNU Assembler (GAS) syntax.
- Emits stack-based code for variables and temporaries and supports `serve`, `sip`, and simple control flow.
- Writes final assembly output to `.s` file.

---

## Project Structure

```text
MiniCCompiler/
├── src/
│   ├── main.cpp                 # Compiler driver and full pipeline orchestration
│   ├── Token.h                  # Token definitions & TokenType enum
│   ├── Lexer.h / Lexer.cpp      # Phase 1: Lexical scanner
│   ├── AST.h                    # AST node hierarchy definitions
│   ├── Parser.h / Parser.cpp    # Phase 2: Recursive Descent Parser
│   ├── SymbolTable.h / .cpp     # Scope-aware symbol table
│   ├── SemanticAnalyzer.h / .cpp  # Phase 3: Semantic validation
│   ├── TAC.h / TAC.cpp          # Phase 5: Three-Address Code structures
│   ├── IRGenerator.h / .cpp     # Phase 5: AST-to-TAC generator
│   ├── Optimizer.h / .cpp       # Phase 6: TAC optimizer
│   ├── CodeGenerator.h / .cpp   # Phase 7: x86 assembly generator
│   └── README doc references
├── examples/
│   ├── valid_program.mc
│   ├── arithmetic_and_precedence.mc
│   ├── optimizations.mc
│   ├── control_flow.mc
│   ├── lexical_error.mc
│   ├── syntax_error.mc
│   ├── semantic_*.mc
│   └── *.s
└── README.md
```

---

## How to Build and Run

### Prerequisites
- Modern C++ compiler (`g++` supporting C++14/C++17, MinGW on Windows).

### 1. Build the Compiler
```bash
g++ -std=c++14 -o MiniCCompiler src/main.cpp src/Lexer.cpp src/Parser.cpp src/SymbolTable.cpp src/SemanticAnalyzer.cpp src/TAC.cpp src/IRGenerator.cpp src/Optimizer.cpp src/CodeGenerator.cpp
```

### 2. Compile a Mini C Source File
```bash
./MiniCCompiler examples/valid_program.mc
```
Or specify a custom output assembly file:
```bash
./MiniCCompiler examples/valid_program.mc -o output.s
```

### 3. Assemble and Run the Generated Assembly
Use `gcc` to assemble the generated x86 assembly into a Windows executable:
```bash
gcc -o valid_program.exe examples/valid_program.s
./valid_program.exe
```

---

## Optimization Example

Given the source code:
```c
brew x = 10 + 20;
brew y = x * 2;
brew z = y + 0;
brew w = z * 1;
brew zero = w * 0;
```

### Intermediate Code (Before Optimization):
```text
t0 = 10 + 20
x = t0
t1 = x * 2
y = t1
t2 = y + 0
z = t2
t3 = z * 1
w = t3
t4 = w * 0
zero = t4
```

### Intermediate Code (After Optimization):
```text
x = 30
y = 60
z = 60
w = 60
zero = 0
```
*(Temporaries are removed when they are no longer necessary and arithmetic is folded or simplified.)*
