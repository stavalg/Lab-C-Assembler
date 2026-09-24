mcro MY_MACRO
    inc r1
mcroend

MY_MACRO: .data 5

BAD_LABEL : .string "hello"

START:  mov #1, r2
        stop
