MOV r0, 0x01
LSH r0, r0, 8
EOR r0, r0, 0x23
LSH r0, r0, 8
EOR r0, r0, 0x45
LSH r0, r0, 8
EOR r0, r0, 0x67
LSH r0, r0, 8
EOR r0, r0, 0x89
LSH r0, r0, 8
EOR r0, r0, 0xab
LSH r0, r0, 8
EOR r0, r0, 0xcd
LSH r0, r0, 8
EOR r0, r0, 0xef

MOV r1, 0xa5
MOV r4, 0x0
LSH r1, r1, 8
EOR r1, r1, 0xa5
ADD r4, r4, 0x1
CMP r4, 0x7
BNE -4
EOR r4, r4, r4

MOV r2, 0xae
LSH r2, r2, 8
EOR r2, r2 0xf4
LSH r2, r2, 8
EOR r2, r2 0x5d
LSH r2, r2, 8
EOR r2, r2 0x74
LSH r2, r2, 8
EOR r2, r2 0x5a
LSH r2, r2, 8
EOR r2, r2 0xff
LSH r2, r2, 8
EOR r2, r2 0x58
LSH r2, r2, 8
EOR r2, r2 0x4f
