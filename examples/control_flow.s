    .file   "examples\control_flow.mc"
    .def    _printf;  .scl    2;  .type   32; .endef
    .def    _puts;    .scl    2;  .type   32; .endef
    .def    _scanf;   .scl    2;  .type   32; .endef
    .section .rdata,"dr"
LC_fmt_int:
    .ascii  "%d\12\0"
LC_fmt_in:
    .ascii  "%d\0"
LC2:
    .ascii  "Grade C\0"
LC3:
    .ascii  "Loop Sum 1 to 5:\0"
LC1:
    .ascii  "Grade B\0"
LC0:
    .ascii  "Grade A\0"

    .text
    .globl  _main
    .def    _main;  .scl    2;  .type   32; .endef
_main:
    pushl   %ebp
    movl    %esp, %ebp
    subl    $48, %esp
    movl    $85, %eax
    movl    %eax, -4(%ebp)
    movl    $0, %eax
    movl    %eax, -8(%ebp)
    movl    $0, %eax
    movl    %eax, -12(%ebp)
    movl    -12(%ebp), %eax
    cmpl    $0, %eax
    je      L1
    movl    $1, %eax
    movl    %eax, -8(%ebp)
    pushl   $LC0
    call    _puts
    addl    $4, %esp
    jmp     L0
L1:
    movl    -4(%ebp), %eax
    movl    $80, %edx
    cmpl    %edx, %eax
    setge    %al
    movzbl  %al, %eax
    movl    %eax, -16(%ebp)
    movl    -16(%ebp), %eax
    cmpl    $0, %eax
    je      L3
    movl    $2, %eax
    movl    %eax, -8(%ebp)
    pushl   $LC1
    call    _puts
    addl    $4, %esp
    jmp     L2
L3:
    movl    $3, %eax
    movl    %eax, -8(%ebp)
    pushl   $LC2
    call    _puts
    addl    $4, %esp
L2:
L0:
    movl    $1, %eax
    movl    %eax, -20(%ebp)
    movl    $0, %eax
    movl    %eax, -24(%ebp)
L4:
    movl    -20(%ebp), %eax
    movl    $5, %edx
    cmpl    %edx, %eax
    setle    %al
    movzbl  %al, %eax
    movl    %eax, -28(%ebp)
    movl    -28(%ebp), %eax
    cmpl    $0, %eax
    je      L5
    movl    -24(%ebp), %eax
    addl    -20(%ebp), %eax
    movl    %eax, -32(%ebp)
    movl    -32(%ebp), %eax
    movl    %eax, -24(%ebp)
    movl    -20(%ebp), %eax
    addl    $1, %eax
    movl    %eax, -36(%ebp)
    movl    -36(%ebp), %eax
    movl    %eax, -20(%ebp)
    jmp     L4
L5:
    pushl   $LC3
    call    _puts
    addl    $4, %esp
    movl    -24(%ebp), %eax
    pushl   %eax
    pushl   $LC_fmt_int
    call    _printf
    addl    $8, %esp
    xorl    %eax, %eax
    leave
    ret
