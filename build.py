import glob
import json5
from quatch import Qvm, Opcode
from zipfile import ZipFile


def patch(in_qvm, symbols, source_glob, includes):
    with open(symbols) as f:
        symbols = json5.load(f)

    qvm = Qvm(in_qvm, symbols=(symbols["code"] | symbols["data"]))

    output = qvm.add_c_files(glob.glob(source_glob), include_dirs=includes, cflags=["-DDEFRAG"])
    if output:
        print(output)

    for symbol in qvm.symbols:
        if symbol.endswith("_H00K"):
            qvm.replace_calls(symbol[: -len("_H00K")], symbol)

    return qvm


print("patching game...")
patched_game = patch(
    "orig_vms/qagame.qvm",
    "src/game/symbols.json5",
    "src/game/*.c",
    ["src/game", "src/sdk/game"],
)
# TODO (#26): Move instruction level patches somewhere else or something
inst = patched_game.instructions[patched_game.symbols["target_laser_start"] + 0x69]
assert inst.opcode == Opcode.CONST and inst.operand == patched_game.symbols["target_laser_think"]
inst.operand = patched_game.symbols["target_laser_think_H00K"]
patched_game.write("build/qagame.qvm", forge_crc=True)

print("patching cgame...")
patched_cgame = patch(
    "orig_vms/cgame.qvm",
    "src/cgame/symbols.json5",
    "src/cgame/*.c",
    ["src/cgame", "src/game", "src/sdk/cgame", "src/sdk/game"],
)
patched_cgame.write("build/cgame.qvm", forge_crc=True)

print("building pk3...")
with ZipFile("build/zzzzz-patched-vms.pk3", "w") as z:
    z.write("build/cgame.qvm", "vm/cgame.qvm")
    z.write("build/qagame.qvm", "vm/qagame.qvm")
