section .data
    const255 dd 255.0          
    const_half dd 0.5          

section .text
    default rel
    bits 64
    global imgCvtGrayFloatToInt


imgCvtGrayFloatToInt:
    push rbp
    mov rbp, rsp
    
    mov rax, rcx               
    mov rbx, rdx              
    mov rcx, r8                
    
    movss xmm1, [rel const255]
    
    movss xmm2, [rel const_half]
    
    test rcx, rcx
    jz done
    
convert_loop:
    movss xmm0, [rax]         

    mulss xmm0, xmm1          
    
    addss xmm0, xmm2          
    
    cvttss2si edx, xmm0    
    
    cmp edx, 0
    jge check_upper
    xor edx, edx             
    jmp store_value
    
check_upper:
    cmp edx, 255
    jle store_value
    mov edx, 255             
    
store_value:
    mov [rbx], dl              
    
    add rax, 4                 
    add rbx, 1                
    
    dec rcx
    jnz convert_loop
    
done:
    pop rbp
    ret