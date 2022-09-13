    PUBLIC _spoofer_stub
     
    .code
     
    _spoofer_stub PROC
        pop r11
        add rsp, 8
        mov rax, [rsp + 24]
        
        mov r10, [rax] ;load shell_param.trampoline
        mov [rsp], r10 ;store address of trampoline as return address
        
        mov r10, [rax + 8] ; load shell_param.function
        mov [rax + 8], r11 ; store the original return address in shell_param.function
     
        mov [rax + 16], r14 ; preserve r14 in shell_param.r14
        lea r14, fixup
        mov [rax], r14 ; store address of fixup label in shell_param.trampoline
        mov r14, rax ; preserve address of shell_param in r14
        
        jmp r10 ; call shell_param.function
     
    fixup:
        sub rsp, 16
        mov rcx, r14 ; restore address of shell_param
        mov r14, [rcx + 16] ; restore r14 from shell_param.r14
        jmp QWORD PTR [rcx + 8] ; jmp to the original return address
    _spoofer_stub ENDP
     
    END