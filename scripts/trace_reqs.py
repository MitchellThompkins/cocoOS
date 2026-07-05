import argparse
import csv
import xml.etree.ElementTree as ET
import sys

from termcolor import colored
from typing import List

def get_args():
    p = argparse.ArgumentParser("trace reqs to test results")

    p.add_argument( '--req',
                    '-r',
                    help='.csv of reqs',
                    required=True )

    p.add_argument( '--test',
                    '-t',
                    nargs='+',
                    help='.xml junit after test run',
                    required=True )

    return p.parse_args()


def trace_tests(req: str, test: List[str]) -> int:
    """
    Gets all test IDs from the system-out section of an xml file which is
    produced after having run the unit tests and compares then to the set of IDs
    provided by the requirements document
    """

    test_id_list = []
    for t in test:
        tree = ET.parse(t)
        root = tree.getroot()

        raw_task_list = root.find("system-out").text
        test_id_list.extend(raw_task_list.split())

    with open(req, 'r') as f:
        reader = csv.DictReader(f)
        req_id_rows = [row['test_case_id'] for row in reader]

    # Detect duplicate requirement IDs
    seen = set()
    duplicates = []
    for rid in req_id_rows:
        if rid in seen:
            duplicates.append(rid)
        seen.add(rid)

    if duplicates:
        print( colored("The following requirement IDs are duplicated in the CSV:", 'yellow') )
        for rid in sorted(set(duplicates)):
            print( colored(f"\t{rid}", 'red') )

    req_list = req_id_rows
    missing_traces = sorted(set(req_list) - set(test_id_list))

    # Warn on trace IDs present in XML but absent from the CSV
    unknown_traces = sorted(set(test_id_list) - set(req_list))
    if unknown_traces:
        print( colored("The following test trace IDs are not in the requirements CSV:", 'yellow') )
        for tid in unknown_traces:
            print( colored(f"\t{tid}", 'yellow') )

    if len(missing_traces) != 0:
        print( colored("The following requirements are not traced to a test:",
                'yellow') )
        for task in missing_traces:
            print( colored(f"\t{task}", 'red') )
    else:
        print( colored("All requirements are traced", 'green'))

    if duplicates or missing_traces:
        return 1
    return 0

if __name__ == '__main__':
    a = get_args()
    result = trace_tests(a.req, a.test)
    sys.exit(result)
