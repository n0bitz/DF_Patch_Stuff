from subprocess import check_output, STDOUT
from zipfile import ZipFile
from shutil import rmtree
from os import makedirs
from sys import executable

rmtree("build", ignore_errors=True)
makedirs("build", exist_ok=True)
check_output([executable, "src/cgame/cgame.py"], stderr=STDOUT)
check_output([executable, "src/qagame/qagame.py"], stderr=STDOUT)
with ZipFile("build/zzzzz-patched-vms.pk3", "w") as z:
    z.write("build/cgame.qvm", "vm/cgame.qvm")
    z.write("build/qagame.qvm", "vm/qagame.qvm")