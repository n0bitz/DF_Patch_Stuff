from patchqvm import *
from struct import pack

output_filename = "qagame_patched.qvm"

qvm = PatchQvm("vm/df1.91.27/qagame.qvm", "vm/g_syscalls.asm")
qvm.rename_var(0x1b2cc, "g_entities")
qvm.rename_var(0x12a2ec, "levelTime")
qvm.rename_var(0x1318b8, "timers")
qvm.rename_sub(0x5d6d, "atof")
qvm.rename_sub(0x5efd, "atoi")
qvm.rename_sub(0x3dba, "get_cheats_enabled")
qvm.rename_sub(0x4586, "placeplayer_teleport")

with open("placeplayer.c", "r") as f:
	symbols = qvm.append_c_code(f.read())
	qvm.replace_code(0x1f6a3, asm("CONST {}".format(symbols["Cmd_PlacePlayer_f"])))
	qvm.add_data_init_code(original_init_address = 0x2b7, original_init_callsite = 0x1e)
	qvm.write(output_filename)