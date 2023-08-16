import argparse
import json
import subprocess
import sys

def cli(tests):

    s = []

    s.append("qemu-system-arm")
    s.append("-nographic")
    s.append("--no-reboot")
    s.append("-serial")
    s.append("stdio")
    s.append("-monitor")
    s.append("none")
    s.append("-machine")
    s.append("xilinx-zynq-a9")
    s.append("-cpu")
    s.append("cortex-a9")
    s.append("-m")
    s.append("12M")
    s.append("--semihosting")
    s.append("-semihosting-config")
    s.append("enable=on,target=native")
    s.append("-kernel")
    s.append("build/a9/test_os_task0.elf")

    print(' '.join(s))

    result = subprocess.run(s)
    sys.exit(result)

def parse_tests(test_file:str):

    with open(test_file, 'r') as f:
        # returns JSON object as
        # a dictionary
        data = json.load(f)

        # Iterating through the json
        # list
        for i in data['tests']:
            print(i)

def get_args():
    p = argparse.ArgumentParser("run tests")
    p.add_argument('--tests_json', '-t', required=True)

    return p.parse_args()


if __name__ == '__main__':
    a = get_args()
    tests = parse_tests(a.tests_json)
    cli(tests)

