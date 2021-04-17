from patchqvm import *
from struct import pack

output_filename = "cgame_patched.qvm"

qvm = PatchQvm("vm/df1.91.27/cgame.qvm", "vm/cg_syscalls.asm")
qvm.rename_sub(0x2f523, "atoi")
qvm.rename_var(0xcfc28, "cg")
qvm.rename_sub(0x1acf4, "CG_ConsoleCommand")
qvm.rename_sub(0x23c, "CG_Printf")
qvm.rename_var(0x11d31c, "decrypted_timer")
qvm.rename_sub(0x324d6, "Q_stricmp")
qvm.rename_sub(0x2f1b8, "strlen")
qvm.rename_sub(0x2f8c7, "vsprintf")

with open("savepos.c", "r") as f:
	symbols = qvm.append_c_code(f.read())
	qvm.replace_code(0x2d, asm("CONST {}".format(symbols["CG_ConsoleCommand_cust"])))
	qvm.replace_data(0x2894, pack("<I", symbols["CG_SavePos_f"]))
	qvm.add_data_init_code(original_init_address = 0xdb9, original_init_callsite = 0x1e)
	qvm.write(output_filename)