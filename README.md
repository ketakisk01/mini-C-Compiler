# Mini C Compiler

A complete end-to-end Mini C Compiler built from scratch in C++. It implements the entire compiler pipeline from lexical analysis through syntax parsing, intermediate code generation (TAC), multi-pass code optimization, and target assembly language generation for x86 (IA-32).

## Architecture & Full Compiler Pipeline

```text
Source Code (.mc)
       ↓
[Phase 1] Lexical Analyzer (Lexer) -> Token Stream + Lexical Error Reporting
       ↓
[Phase 2] Syntax Analyzer (Parser) -> Recursive-Descent Parsing + Parse Tree & AST
       ↓
[Phase 3] Symbol Table -> Identifier, Scope, and Line Tracking
       ↓
[Phase 4] Intermediate Code Generator (IR) -> Three-Address Code (TAC)
       ↓
[Phase 5] Code Optimizer -> Multi-Pass Constant Folding, Propagation, Algebraic Simplification & DCE
       ↓
[Phase 6] Assembly Code Generator -> Target x86 (IA-32 GAS) Assembly (.s)
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
- Implements LL(1)-compatible recursive descent parsing.
- Handles operator precedence via cascading grammar rules.
- Builds both a detailed **Parse Tree** (`ParseTreeNode`) and an **Abstract Syntax Tree (AST)** (`ASTNode` hierarchy).
- Gracefully handles syntax errors, reports line/column context, and prevents code generation on invalid code.

### Phase 3: Symbol Table (`SymbolTable.h`, `SymbolTable.cpp`)
- Tracks declared variables, kinds, and declaration lines.

### Phase 4: Intermediate Code Generation (`TAC.h`, `TAC.cpp`, `IRGenerator.h`, `IRGenerator.cpp`)
- Translates AST into linear **Three-Address Code (TAC)**.
- Operates on instruction forms:
  - Binary arithmetic/relational/logical: `t = a OP b`
  - Unary operations: `t = - a`, `t = ! a`
  - Copies: `x = y`
  - Branching: `ifFalse t goto L`, `if t goto L`, `goto L`
  - Labels: `L:`
  - Built-ins: `serve "str"`, `serve x`, `sip x`, `return x`
- Generates clean, human-readable intermediate representations.

### Phase 5: Code Optimization (`Optimizer.h`, `Optimizer.cpp`)
Operates directly on TAC instructions using a multi-pass fixpoint loop:
1. **Constant Folding**: Evaluates compile-time expressions (arithmetic, comparisons, logical operations, unary negations).
2. **Constant Propagation**: Tracks constant bindings within basic blocks and propagates them into operand positions.
3. **Algebraic Simplification**: Eliminates arithmetic identities (`x + 0 -> x`, `x - 0 -> x`, `x * 1 -> x`, `x * 0 -> 0`, `x / 1 -> x`).
4. **Dead Code Elimination (DCE)**:
   - Removes unused temporary variables (`tN`).
   - Eliminates dead instructions after unconditional jumps (`goto`, `return`).
   - Resolves branches with known boolean conditions (`ifFalse 0 -> goto`, `ifFalse 1 -> removed`).
   - Removes redundant jumps (`goto L` immediately followed by `L:`).
- Displays intermediate code before and after optimization along with optimization metrics.

### Phase 6: Target Assembly Generation (`CodeGenerator.h`, `CodeGenerator.cpp`)
- Generates **x86 (IA-32) standard GNU Assembler (GAS)** syntax compatible with MinGW GCC on Windows.
- **Stack-Frame Strategy**:
  - Dynamic 16-byte aligned stack allocation (`subl $N, %esp`).
  - Base pointer relative offsets (`-4(%ebp)`, `-8(%ebp)`, ...) for all variables and temporaries.
- **Data Section**:
  - Read-only data `.section .rdata,"dr"` for format strings and user string literals.
- **Instruction Mapping**:
  - Arithmetic and logic mapped to `movl`, `addl`, `subl`, `imull`, `idivl`, `cmpl`, `sete`/`setne`/`setl`/`setle`/`setg`/`setge`.
  - Jump instructions mapped to `jmp`, `je`, `jne`.
  - Built-ins mapped to standard C runtime calls (`_puts`, `_printf`, `_scanf`).
- Writes final assembly output to `.s` file.

---

## Project Structure

```text
MiniCCompiler/
├── src/
│   ├── main.cpp                 # CLI Compiler Driver (orchestrates pipeline)
│   ├── Token.h                  # Token definitions & TokenType enum
│   ├── Lexer.h / Lexer.cpp      # Phase 1: Lexical scanner
│   ├── AST.h                    # AST node hierarchy definitions
│   ├── Parser.h / Parser.cpp    # Phase 2: Recursive Descent Parser
│   ├── SymbolTable.h / .cpp     # Phase 3: Scoped Symbol Table
│   ├── TAC.h / TAC.cpp          # Phase 4: Three-Address Code structures
│   ├── IRGenerator.h / .cpp     # Phase 4: AST-to-TAC IR Generator
│   ├── Optimizer.h / .cpp       # Phase 5: TAC Multi-pass Optimizer
│   └── CodeGenerator.h / .cpp   # Phase 6: x86 Target Assembly Generator
├── examples/
│   ├── valid_program.mc         # Control flow, loops, and string serve
│   ├── arithmetic_and_precedence.mc # Precedence, binary arithmetic expressions
│   ├── optimizations.mc         # Constant folding, propagation, algebraic simplification
│   ├── control_flow.mc          # Nested conditionals and while iteration
│   ├── lexical_error.mc         # Error test: invalid characters
│   └── syntax_error.mc          # Error test: malformed syntax
└── README.md                    # Project documentation
```

---

## How to Build and Run

### Prerequisites
- Modern C++ compiler (`g++` supporting C++14/C++17, MinGW on Windows).

### 1. Build the Compiler
```bash
g++ -std=c++14 -o MiniCCompiler src/main.cpp src/Lexer.cpp src/Parser.cpp src/SymbolTable.cpp src/TAC.cpp src/IRGenerator.cpp src/Optimizer.cpp src/CodeGenerator.cpp
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
*(All temporary variables `t0`–`t4` eliminated, expressions folded and propagated)*
