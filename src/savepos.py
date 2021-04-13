from patchqvm import *
from struct import pack

output_filename = "cgame_patched.qvm"

qvm = PatchQvm("vm/df1.91.27/cgame.qvm", "vm/cg_syscalls.asm")
qvm.rename_sub(0x2f523, "atoi")
qvm.rename_sub(0x324d, "Q_stricmp")
qvm.rename_sub(0x23c, "CG_Printf")
qvm.rename_sub(0x2f1b8, "strlen")
qvm.rename_sub(0x77b6, "sprintf")
qvm.rename_var(0xcfc4c - 36, "cg")
qvm.rename_var(0x11d31c, "decrypted_timer")

with open("savepos.c", "r") as f:
	symbols = qvm.append_c_code(f.read())
	qvm.replace_data(0x2894, pack("<I", symbols["CG_SavePos_f"]))
	qvm.add_data_init_code(original_init_address = 0xdb9, original_init_callsite = 0x1e)
	qvm.write(output_filename)