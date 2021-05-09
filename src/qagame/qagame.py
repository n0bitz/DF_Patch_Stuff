from quatch import Qvm
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
	"fire_grapple": 0x228ee,
	"G_InitGame": 0x2b7,
	"get_cheats_enabled": 0x3dba,
	"placeplayer_teleport": 0x4586,
	"Q_stricmp": 0xa9fa
}
with open("syscalls.json", "r") as f:
	symbols.update(load_json(f))
qvm = Qvm("../../vms/DF 1.91.27/qagame.qvm", symbols)
with open("qagame.c", "r") as f:
	qvm.add_c_code(f.read(), ["../vanilla_29_sdk/cgame", "../vanilla_29_sdk/game"])
qvm.replace_calls("ClientCommand", "ClientCommand_Hook")
qvm.replace_calls("fire_grapple", "fire_grapple_Hook")
qvm.write("qagame.qvm")