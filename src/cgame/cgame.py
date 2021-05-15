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
	qvm.add_c_code(f.read(), ["../vanilla_29_sdk/cgame", "../vanilla_29_sdk/game"])
qvm.replace_calls("CG_ConsoleCommand", "CG_ConsoleCommand_Hook")
qvm.replace_calls("CG_InitConsoleCommands", "CG_InitConsoleCommands_Hook")
qvm.replace_calls("CG_Grapple", "CG_Grapple_Hook")
qvm.replace_calls("SomeNoDrawRadiusChecks", "SomeNoDrawRadiusChecks_Hook")
qvm.write("cgame.qvm")