import argparse
import json
import os
import subprocess
import sys


class Test:
    def __init__(self, test_def):
        self.configure(test_def)
        self.exes = self.get_test_executables()

    def configure(self, test_def):
        self.command = test_def["type"]
        self.dir = test_def["dir"]

        if self.command != "host":
            self.machine = test_def["machine"]
            self.cpu = test_def["cpu"]
            self.memory = test_def["memory"]
        else:
            self.machine = None
            self.cpu = None
            self.memory = None

    def get_test_executables(self):
        test_exes = []
        for f in os.listdir(f"{self.dir}"):
            if f.endswith(".elf"):
                test_exes.append(os.path.abspath( os.path.join(f"{self.dir}", f) ))

        return test_exes

    def run_tests(self):
        results = []
        for exe in self.exes:
            s = self._assemble_command(exe)
            print(f"\n{' '.join(s)}\n")
            results.append( subprocess.run(s).returncode)

        return results

    def _assemble_command(self, test_exe):
        s = []

        if self.command == "host":
            s.append(f"{test_exe}")
        else:
            s.append("qemu-system-arm")
            s.append("-nographic")
            s.append("--no-reboot")
            s.append("-serial")
            s.append("stdio")
            s.append("-monitor")
            s.append("none")
            s.append("-machine")
            s.append(f"{self.machine}")
            s.append("-cpu")
            s.append(f"{self.cpu}")
            s.append("-m")
            s.append(f"{self.memory}")
            s.append("--semihosting")
            s.append("-semihosting-config")
            s.append("enable=on,target=native")
            s.append("-kernel")
            s.append(f"{test_exe}")

        return s


def parse_tests(test_def_json:str):
    """
    Parse json file to extract test configurations
    """
    with open(test_def_json, 'r') as f:
        data = json.load(f)
        test_list = [Test(i) for i in data['tests']]

    return test_list


def cli(tests):
    results = []

    for t in tests:
        results.append( t.run_tests() )

    for r in results:
        for test_result in r:
            if test_result != 0:
                sys.exit(test_result)

    sys.exit(0)


def get_args():
    p = argparse.ArgumentParser("run tests")
    p.add_argument('--tests_json', '-t', required=True)

    return p.parse_args()

if __name__ == '__main__':
    a = get_args()
    tests = parse_tests(a.tests_json)
    cli(tests)
