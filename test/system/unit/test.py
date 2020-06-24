#!/usr/bin/python3

from argparse import ArgumentParser
from colorama import init
from tucuxi.processing.tucuxirun import *
import filecmp


parser = ArgumentParser(description='Tucuxi system test concerning data unit.',
                        prog='test.py')

# For colorama, to reset the color on every new print
init(autoreset=True)

foldername = ''

app_path = os.path.dirname(os.path.realpath(__file__))


def parse_the_args():
    """
    * This method parses the command line arguments. This project relies on windows having unix tools
      available in the console. We get this by installing git and allowing to use it via cmd.exe.
    """
    global app_path
    cli = 'tucucli'
    drgsfldr = os.path.join(app_path, "..", "drugfiles")

    parser.add_argument('-d', type=str, dest='drugspath',
                        help='Import folder paths for Tucuxi drugfiles (tdd)',
                        default=drgsfldr)
    parser.add_argument('-t', '--tucucli', type=str, dest='tucucli',
                        help='Command to execute Tucuxi cli, default is tucucli',
                        default=cli)
    return parser.parse_args()


def process_queryfile(args, query_file: str, output_folder: str, query_id):
    test_folder_name = query_file.split(os.path.sep)[-3]
    test_query_name = query_file.split(os.path.sep)[-1]
    print("\nRunning file '{qid}' from folder '{testfolder}'".format(qid=test_query_name,
                                                                     testfolder=test_folder_name))
    tucucli = args.tucucli
    drug_path = args.drugspath
    output_file_path = output_folder + '/' + "query" + str(query_id + 1) + '.xml'

    tucuclirun = TucuCliRun(tucucli, drug_path, output_file_path)
    tucuclirun.run_tucuxi_from_file(query_file)


def compare_query_responses(queries):
    once = True
    print('\nRunning responses comparison ...')
    for i, test in enumerate(queries):
        test_folder_name = test[1].split(os.path.sep)[-2]
        for r, d, f in os.walk(test[1]):
            for file in f:
                if not filecmp.cmp(os.path.join(r, file), os.path.join(r, f[0]), shallow=False) and once:
                    print(Fore.RED + "{} : queries responses not equal".format(test_folder_name))
                    once = False
        once = True
    print("Comparisons finished")


def get_query_responses(args, queries):
    for test in queries:
        for qid, queryfile in enumerate(test[0]):
            process_queryfile(args, queryfile, test[1], qid)


def get_queries_path(query_responses):
    global app_path

    for r, d, f in os.walk(app_path):
        for i, dir1 in enumerate(d):
            for r1, d1, f1 in os.walk(os.path.join(r, dir1)):
                for i1, dir2 in enumerate(d1):
                    if dir2 == "in_tqf":
                        query_responses.append([[]])
                        for r2, d2, f2 in os.walk(os.path.join(r1, dir2)):
                            for file in f2:
                                query_responses[i][i1].append(os.path.join(r2, file))
                    else:
                        query_responses[i].append(os.path.join(r1, dir2))


def main():
    args = parse_the_args()

    queries = []

    get_queries_path(queries)

    get_query_responses(args, queries)

    compare_query_responses(queries)

    print(Fore.CYAN + 'End of script')


if __name__ == "__main__":
    main()
