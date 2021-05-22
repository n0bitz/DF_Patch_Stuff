from quatch import Qvm
from struct import pack
from json import load as load_json

symbols = {
    # DATA
    "cg": 0xcfc28,
    "decrypted_timer": 0x11d31c,
    # CODE
    "atoi": 0x2f523,
    "AxisClear": 0x30e90,
    "CG_ConsoleCommand": 0x1acf4,
    "CG_Beam": 0x1ee8b,
    "CG_Grapple": 0x1eda6,
    "CG_Init": 0xdb9,
    "CG_InitConsoleCommands": 0x1ad32,
    "CG_Printf": 0x23c,
    "DecryptTimer": 0x5f5e,
    "SomeNoDrawRadiusChecks": 0x9bb2,
    "Q_stricmp": 0x324d6,
    "strlen": 0x2f1b8,
    "vsprintf": 0x2f8c7
}

with open("syscalls.json", "r") as f:
    symbols.update(load_json(f))

qvm = Qvm("../../vms/DF 1.91.27/cgame.qvm", symbols)
with open("cgame.c", "r") as f:
    qvm.add_c_code(f.read(), ["../sdk/cgame", "../sdk/game"])

for sym in qvm.symbols:
    i = sym.rfind("_Hook")
    if i >= 0:
        qvm.replace_calls(sym[:i], sym)

qvm.write("cgame.qvm", forge_crc=True)
