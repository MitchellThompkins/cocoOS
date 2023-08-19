import click
import csv
import xml.etree.ElementTree as ET
import sys

from termcolor import colored


@click.command()
@click.option(
    "--req",
    help=f".csv of reqs",
    required=True,
)
@click.option(
    "--test",
    help=f".xml junit after test run",
    required=True,
)
def cli(req, test):
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

    sys.exit(0)

if __name__ == '__main__':
    cli()
