mcro my_macro
    mov r3, r4
    inc r2
mcroend

LOOP: .data 10, -5, 20
.string "Hello"

my_macro

jmp LOOP
stop
