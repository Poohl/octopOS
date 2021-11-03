
mov r0, #'a'
ldr r1,=(0x00FFFFF200+7*4)
str r0, [r1]
