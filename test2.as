; file ps.as

.extern LENGTH 

L3: mov @r3 ,LENGTH
W: jmp LENGTH
.entry L3
.entry W

