from quatch import Qvm
from quatch.instruction import Opcode
from json import load as load_json

symbols = {
    # DATA
    "g_entities": 0x1b2cc,
    "levelTime": 0x12a2ec,
    "timers": 0x1318b8,
    # CODE
    "atof": 0x5d6d,
    "atoi": 0x5efd,
    "ClientCommand": 0x1f3c3,
    "ClientSpawnEntSetStuff": 0x2722,
    "fire_grapple": 0x228ee,
    "G_Damage": 0x20074,
    "G_InitGame": 0x2b7,
    "G_Say": 0x1e51d,
    "get_cheats_enabled": 0x3dba,
    "placeplayer_teleport": 0x4586,
    "Q_stricmp": 0xa9fa,
    "target_laser_start": 0x2576d,
    "target_laser_think": 0x255c8,
    "VectorNormalize": 0x9c59,
    "G_Printf": 0x96
}
with open("syscalls.json", "r") as f:
    symbols.update(load_json(f))

qvm = Qvm("../../vms/DF 1.91.27/qagame.qvm", symbols)
with open("qagame.c", "r") as f:
    qvm.add_c_code(f.read(), ["../sdk/cgame", "../sdk/game"])

for sym in qvm.symbols:
    i = sym.rfind("_Hook")
    if i >= 0:
        qvm.replace_calls(sym[:i], sym)

inst = qvm.instructions[symbols["target_laser_start"] + 0x69]
assert(inst.opcode == Opcode.CONST and inst.operand ==
       symbols["target_laser_think"])
inst.operand = qvm.symbols["target_laser_think_Hook"]

qvm.write("qagame.qvm", forge_crc=True)
