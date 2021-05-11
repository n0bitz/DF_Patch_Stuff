Disclaimer:
	I've barely spent any time reading lcc source code, and have no idea if my patches are correct.
	I've also barely tested them, but they seem to work so far. ¯\_(ツ)_/¯

The gist is to basically reuse the existing symbols and types for typedefs and structs.
Otherwise, we'll probably run into errors like, `conflicting types: <blah> and <blah>`.
As, the things parsed prior to the redefinition/redeclaration are using the old symbols/types.
When redefining structs, the field data for the existing symbol is wiped to avoid redefining field errors.
Similarly, when retypedef-ing, the new type is copied onto the existing symbol.

`rcc.patch` is a diff against Cyrax's q3lcc (https://github.com/ec-/q3lcc).

DeFRaG, as far as I can tell, uses 1.17 SDK's rcc and I couldn't find the source code for that rcc.
So `rcc.exe` was the result of me reversing and binary patching 1.17 SDK rcc manually.
Pretty much the same thing as the source level patch was done for the binary patch, except I wrote over the calls to error.

decl typedef case patch:
        00002c17 60              PUSHAD
        00002c18 8b 79 24        MOV        EDI, dword ptr [ECX + 0x24]
        00002c1b 8b 75 f8        MOV        ESI, dword ptr [EBP + -0x8]
        00002c1e 31 c9           XOR        ECX, ECX
        00002c20 b1 28           MOV        CL, 0x28
        00002c22 f3 a4           MOVSB.REP  ES:EDI, ESI
        00002c24 61              POPAD
        00002c25 eb 3c           JMP        LAB_00002c63




newstruct patch:
        0000f293 31 c9           XOR        ECX, ECX
        0000f295 89 4a 30        MOV        dword ptr [EDX + 0x30], ECX
        0000f298 89 4a 34        MOV        dword ptr [EDX + 0x34], ECX
        0000f29b 89 4a 38        MOV        dword ptr [EDX + 0x38], ECX
        0000f29e 8b 42 24        MOV        EAX, dword ptr [EDX + 0x24]
        0000f2a1 e9 7d 00        JMP        LAB_0000f323
                 00 00


