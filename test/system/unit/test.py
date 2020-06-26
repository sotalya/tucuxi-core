#!/usr/bin/python3

from argparse import ArgumentParser
from colorama import init
from tucuxi.processing.tucuxirun import *
import filecmp
import sys
from os.path import isfile, join
from os import listdir

target_evaluation = []

parser = ArgumentParser(description='Tucuxi system test concerning data unit.',
                        prog='test.py')

# For colorama, to reset the color on every new print
init(autoreset=True)

app_path = os.path.dirname(os.path.realpath(__file__))
output_folder_name = "output"
input_folder_name = "in_tqf"
query_responses = []


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
    parser.add_argument('-b', '--target', type=bool, dest='target',
                        help='Command to execute Tucuxi cli, default is tucucli',
                        default=False)
    return parser.parse_args()


def process_queryfile(args, query_file: str, output_folder: str, index):
    """
    Input : args, query_file, output_folder
    Generate query response for each query
    """
    global target_evaluation
    global query_responses

    test_folder_name = query_file.split(os.path.sep)[-3]
    test_query_name = query_file.split(os.path.sep)[-1]
    print("\nRunning file '{qid}' from folder '{testfolder}'".format(qid=test_query_name,
                                                                     testfolder=test_folder_name))
    tucucli = args.tucucli
    drug_path = args.drugspath
    output_file_path = output_folder + '/' + test_query_name + '.xml'

    tucuclirun = TucuCliRun(tucucli, drug_path, output_file_path)
    query_responses.append(tucuclirun.run_tucuxi_from_file(query_file))

    if hasattr(query_responses[index], "responses"):
        for response in query_responses[index].responses:
            if hasattr(response.computingTrait, "adjustments"):
                for ad in response.computingTrait.adjustments:
                    if hasattr(ad, "targetEvaluations"):
                        for t in ad.targetEvaluations:
                            t.unit = None
                            t.value = 0.0
                            target_evaluation.append(index)


def compare_query_responses(args, queries):
    """
    Input : queries
    Compare the first query response to the other (for each test)
    """
    global query_responses
    global target_evaluation

    index = 0
    first_index = 0
    once = True
    once_target = True
    print('\nRunning responses comparison ...')
    for i, test in enumerate(queries):
        test_folder_name = test[1].split(os.path.sep)[-2]
        for r, d, f in os.walk(test[1]):
            for j, file in enumerate(f):
                if len(target_evaluation) != 0 and (index in target_evaluation):
                    first_index = index - j
                    if query_responses[index] != query_responses[first_index] and not once_target:
                        print(Fore.RED + "{} : queries responses not equal".format(test_folder_name))
                        once_target = False
                else:
                    if not filecmp.cmp(os.path.join(r, file), os.path.join(r, f[0]), shallow=False) and once:
                        print(Fore.RED + "{} : queries responses not equal".format(test_folder_name))
                        once = False
                index += 1
        once = True
        once_target = True
    print("Comparisons finished")


def get_query_responses(args, queries):
    index = 0
    for test in queries:
        for queryfile in test[0]:
            process_queryfile(args, queryfile, test[1], index)
            index += 1


def get_queries_path(queries):
    """
    Input : empty queries list
    Fill queries list with all tests. Each test contain a list of queries path and the output path
    """

    global app_path
    global input_folder_name
    global output_folder_name
    tests_path = os.path.join(app_path, "tests")

    list_test_folder = [f.name for f in os.scandir(tests_path) if f.is_dir()]
    for ti, testFolder in enumerate(list_test_folder):
        queries.append([[]])
        path_to_test_folder = os.path.join(tests_path, testFolder)
        path_to_queries = os.path.join(path_to_test_folder, input_folder_name)
        if not os.path.exists(path_to_queries):
            print(Fore.RED + "Missing '{miss}' folder in {test}".format(miss=input_folder_name, test=testFolder))
            sys.exit(1)
        path_to_output = os.path.join(path_to_test_folder, output_folder_name)
        if not os.path.exists(path_to_output):
            os.mkdir(path_to_output)
        list_files = [f for f in listdir(path_to_queries) if isfile(join(path_to_queries, f))]
        queries[ti].append(os.path.join(tests_path, testFolder, output_folder_name))
        for fi, file in enumerate(list_files):
            queries[ti][0].append(os.path.join(path_to_queries, file))


def main():
    args = parse_the_args()

    queries = []

    get_queries_path(queries)

    get_query_responses(args, queries)

    compare_query_responses(args, queries)

    print(Fore.CYAN + 'End of script')


if __name__ == "__main__":
    main()
