from quatch import Qvm
from json import load as load_json
from pathlib import PurePath

DIR = PurePath(__file__).parent

symbols = {
    # DATA
    "cg": 0xcfc28,
    "decrypted_timer": 0x11d31c,
    # CODE
    "atoi": 0x2f523,
    "AxisClear": 0x30e90,
    "CG_ConsoleCommand": 0x1acf4,
    "CG_ProcessSnapshots": 0x291f9,
    "CG_Beam": 0x1ee8b,
    "CG_Grapple": 0x1eda6,
    "CG_Init": 0xdb9,
    "CG_InitConsoleCommands": 0x1ad32,
    "CG_DrawActive": 0x1cd8b,
    "CG_Printf": 0x23c,
    "DecryptTimer": 0x5f5e,
    "SomeNoDrawRadiusChecks": 0x9bb2,
    "Q_stricmp": 0x324d6,
    "strlen": 0x2f1b8,
    "vsprintf": 0x2f8c7,
    "AnglesToAxis": 0x30e5b
}

with open(DIR.joinpath("syscalls.json"), "r") as f:
    symbols.update(load_json(f))

qvm = Qvm(DIR.joinpath("../../orig_vms/cgame.qvm"), symbols)
with open(DIR.joinpath("cgame.c"), "r") as f:
    qvm.add_c_code(f.read(), [DIR, DIR.joinpath("../sdk/cgame"), DIR.joinpath("../sdk/game")])

for sym in qvm.symbols:
    i = sym.rfind("_Hook")
    if i >= 0:
        qvm.replace_calls(sym[:i], sym)

qvm.write(DIR.joinpath("../../build/cgame.qvm"), forge_crc=True)
