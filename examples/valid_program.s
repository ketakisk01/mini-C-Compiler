    .file   "examples\valid_program.mc"
    .def    _printf;  .scl    2;  .type   32; .endef
    .def    _puts;    .scl    2;  .type   32; .endef
    .def    _scanf;   .scl    2;  .type   32; .endef
    .section .rdata,"dr"
LC_fmt_int:
    .ascii  "%d\12\0"
LC_fmt_in:
    .ascii  "%d\0"
LC1:
    .ascii  "Fail\0"
LC3:
    .ascii  "Program Finished\0"
LC2:
    .ascii  "Good Attendance\0"
LC0:
    .ascii  "Pass\0"

    .text
    .globl  _main
    .def    _main;  .scl    2;  .type   32; .endef
_main:
    pushl   %ebp
    movl    %esp, %ebp
    subl    $16, %esp
    movl    $78, %eax
    movl    %eax, -4(%ebp)
    movl    $85, %eax
    movl    %eax, -8(%ebp)
    pushl   $LC0
    call    _puts
    addl    $4, %esp
    jmp     L0
L1:
    pushl   $LC1
    call    _puts
    addl    $4, %esp
L0:
L2:
    movl    -8(%ebp), %eax
    movl    $80, %edx
    cmpl    %edx, %eax
    setg    %al
    movzbl  %al, %eax
    movl    %eax, -12(%ebp)
    movl    -12(%ebp), %eax
    cmpl    $0, %eax
    je      L3
    pushl   $LC2
    call    _puts
    addl    $4, %esp
    movl    -8(%ebp), %eax
    subl    $5, %eax
    movl    %eax, -16(%ebp)
    movl    -16(%ebp), %eax
    movl    %eax, -8(%ebp)
    jmp     L2
L3:
    pushl   $LC3
    call    _puts
    addl    $4, %esp
    xorl    %eax, %eax
    leave
    ret
