from subprocess import check_output, STDOUT, CalledProcessError
from zipfile import ZipFile
from shutil import rmtree
from os import makedirs
from sys import executable

rmtree("build", ignore_errors=True)
makedirs("build", exist_ok=True)
def build_vm(vm):
    print(f"{vm}:")
    try:
        print(check_output([executable, f"src/{vm}/{vm}.py"], stderr=STDOUT).decode("utf8"))
    except CalledProcessError as e:
        print(e.output.decode("utf8"))
    print("---------------------------------------------------------------------------")

build_vm("cgame")
build_vm("qagame")
with ZipFile("build/zzzzz-patched-vms.pk3", "w") as z:
    z.write("build/cgame.qvm", "vm/cgame.qvm")
    z.write("build/qagame.qvm", "vm/qagame.qvm")