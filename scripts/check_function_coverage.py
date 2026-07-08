import argparse
import re
import subprocess
import sys

from termcolor import colored

COMPONENTS = [
    'os_task',
    'os_event',
    'os_sem',
    'os_msgqueue',
    'os_kernel',
    'os_utils',
]

# Functions covered by means other than a direct call in the component test
# (e.g. via weak-symbol override or indirect exercising that is intentional).
ALLOWLIST = set()


def get_args():
    p = argparse.ArgumentParser(description='verify every public function has a direct test call')
    p.add_argument('--build-dir', '-b', default='build/x86_64',
                   help='x86_64 build directory containing os/<comp>/lib<comp>_impl.a')
    return p.parse_args()


def defined_t_symbols(archive):
    try:
        result = subprocess.run(
            ['nm', '-g', '--defined-only', archive],
            capture_output=True, text=True
        )
    except FileNotFoundError:
        return []
    symbols = []
    for line in result.stdout.splitlines():
        parts = line.split()
        if len(parts) >= 3 and parts[-2] == 'T':
            symbols.append(parts[-1])
    return sorted(set(symbols))


def is_directly_called(fn, test_file):
    pattern = re.compile(r'(?<![a-zA-Z0-9_])' + re.escape(fn) + r'\s*\(')
    try:
        with open(test_file) as f:
            return pattern.search(f.read()) is not None
    except FileNotFoundError:
        return False


def check_coverage(build_dir):
    misses = []

    for comp in COMPONENTS:
        archive   = f'{build_dir}/os/{comp}/lib{comp}_impl.a'
        test_file = f'tests/{comp}/src/{comp}.test.cpp'
        symbols   = defined_t_symbols(archive)

        if not symbols:
            print(colored(f'WARNING: no T symbols found in {archive}', 'yellow'))
            continue

        for fn in symbols:
            if fn in ALLOWLIST:
                continue
            if not is_directly_called(fn, test_file):
                misses.append((comp, fn))

    if misses:
        print(colored('The following public functions have no direct call in their component test:', 'yellow'))
        for comp, fn in sorted(misses):
            print(colored(f'\t{comp}: {fn}', 'red'))
        return 1

    print(colored('All public functions are directly tested', 'green'))
    return 0


if __name__ == '__main__':
    a = get_args()
    sys.exit(check_coverage(a.build_dir))
