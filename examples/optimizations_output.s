    .file   "examples\optimizations.mc"
    .def    _printf;  .scl    2;  .type   32; .endef
    .def    _puts;    .scl    2;  .type   32; .endef
    .def    _scanf;   .scl    2;  .type   32; .endef
    .section .rdata,"dr"
LC_fmt_int:
    .ascii  "%d\12\0"
LC_fmt_in:
    .ascii  "%d\0"

    .text
    .globl  _main
    .def    _main;  .scl    2;  .type   32; .endef
_main:
    pushl   %ebp
    movl    %esp, %ebp
    subl    $32, %esp
    movl    $30, %eax
    movl    %eax, -4(%ebp)
    movl    $60, %eax
    movl    %eax, -8(%ebp)
    movl    $60, %eax
    movl    %eax, -12(%ebp)
    movl    $60, %eax
    movl    %eax, -16(%ebp)
    movl    $0, %eax
    movl    %eax, -20(%ebp)
    movl    $30, %eax
    pushl   %eax
    pushl   $LC_fmt_int
    call    _printf
    addl    $8, %esp
    movl    $60, %eax
    pushl   %eax
    pushl   $LC_fmt_int
    call    _printf
    addl    $8, %esp
    movl    $60, %eax
    pushl   %eax
    pushl   $LC_fmt_int
    call    _printf
    addl    $8, %esp
    movl    $60, %eax
    pushl   %eax
    pushl   $LC_fmt_int
    call    _printf
    addl    $8, %esp
    movl    $0, %eax
    pushl   %eax
    pushl   $LC_fmt_int
    call    _printf
    addl    $8, %esp
    xorl    %eax, %eax
    leave
    ret
