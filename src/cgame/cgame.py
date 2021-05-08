from quatch import Qvm
from struct import pack
from json import load as load_json

symbols = {
	# DATA
	"cg": 0xcfc28,
	"decrypted_timer": 0x11d31c,
	# CODE
	"atoi": 0x2f523,
	"CG_ConsoleCommand": 0x1acf4,
	"CG_Init": 0xdb9,
	"CG_Printf": 0x23c,
	"DecryptTimer": 0x5f5e,
	"Q_stricmp": 0x324d6,
	"strlen": 0x2f1b8,
	"vsprintf": 0x2f8c7
}
with open("syscalls.json", "r") as f:
	symbols.update(load_json(f))
qvm = Qvm("../../vms/DF 1.91.27/cgame.qvm", symbols)
with open("cgame.c", "r") as f:
	qvm.add_c_code(f.read(), ["../vanilla_29_sdk/cgame", "../vanilla_29_sdk/game"])
qvm.replace_calls("CG_ConsoleCommand", "CG_ConsoleCommand_cust")
# TODO (#9): Nuke below two lines please
qvm.data = bytearray(qvm.data)
qvm.data[0x2894:0x2898] = pack("<I", qvm.symbols["CG_SavePos_f"])
qvm.write("cgame.qvm")