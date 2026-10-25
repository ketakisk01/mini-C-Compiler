    .file   "examples\valid.mc"
    .def    _printf;  .scl    2;  .type   32; .endef
    .def    _puts;    .scl    2;  .type   32; .endef
    .def    _scanf;   .scl    2;  .type   32; .endef
    .section .rdata,"dr"
LC_fmt_int:
    .ascii  "%d\12\0"
LC_fmt_in:
    .ascii  "%d\0"
LC1:
    .ascii  "Sum is 10 or less\0"
LC0:
    .ascii  "Sum is greater than 10\0"

    .text
    .globl  _main
    .def    _main;  .scl    2;  .type   32; .endef
_main:
    pushl   %ebp
    movl    %esp, %ebp
    subl    $32, %esp
    movl    $10, %eax
    movl    %eax, -4(%ebp)
    movl    $5, %eax
    movl    %eax, -8(%ebp)
    movl    $15, %eax
    movl    %eax, -12(%ebp)
    movl    $15, %eax
    pushl   %eax
    pushl   $LC_fmt_int
    call    _printf
    addl    $8, %esp
    pushl   $LC0
    call    _puts
    addl    $4, %esp
    jmp     L0
L1:
    pushl   $LC1
    call    _puts
    addl    $4, %esp
L0:
    movl    $0, %eax
    movl    %eax, -16(%ebp)
L2:
    movl    -16(%ebp), %eax
    movl    $3, %edx
    cmpl    %edx, %eax
    setl    %al
    movzbl  %al, %eax
    movl    %eax, -20(%ebp)
    movl    -20(%ebp), %eax
    cmpl    $0, %eax
    je      L3
    movl    -16(%ebp), %eax
    pushl   %eax
    pushl   $LC_fmt_int
    call    _printf
    addl    $8, %esp
    movl    -16(%ebp), %eax
    addl    $1, %eax
    movl    %eax, -24(%ebp)
    movl    -24(%ebp), %eax
    movl    %eax, -16(%ebp)
    jmp     L2
L3:
    movl    $0, %eax
    leave
    ret
