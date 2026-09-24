mcro my_macro
    mov r3, r4
mcroend

LOOP: .data 10
LOOP: .string "Hello"

my_macro
stop
