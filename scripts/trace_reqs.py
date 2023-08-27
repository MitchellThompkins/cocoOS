import argparse
import csv
import xml.etree.ElementTree as ET
import sys

from termcolor import colored

def get_args():
    p = argparse.ArgumentParser("trace reqs to test results")
    p.add_argument('--req', '-r', help='.csv of reqs',  required=True)
    p.add_argument('--test', '-t', help='.xml junit after test run',  required=True)

    return p.parse_args()


def trace_tests(req, test) -> int:
    tree = ET.parse(test)
    root = tree.getroot()

    raw_task_list = root.find("system-out").text
    task_list = raw_task_list.split()

    with open(req, 'r') as f:
        reader = csv.DictReader(f)
        req_list = [row['test_case_id'] for row in reader]

    missing_traces = sorted(set(req_list) - set(task_list))

    if len(missing_traces) != 0:
        print( colored("The following requirements are not traced to a test:",
                'yellow') )
        for task in missing_traces:
            print( colored(f"\t{task}", 'red') )
    else:
        print( colored("All requirements are traced", 'green'))

    return 0

if __name__ == '__main__':
    a = get_args()
    result = trace_tests(a.req, a.test)
    sys.exit(result)
