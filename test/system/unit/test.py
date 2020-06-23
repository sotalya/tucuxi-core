#!/usr/bin/python3

import sys
from argparse import ArgumentParser
from subprocess import CalledProcessError
from colorama import init
import xml.dom.minidom
from tucuxi.processing.tucuxirun import *
from os import listdir
from os.path import isfile, join
import filecmp

# globals

parser = ArgumentParser(description='Tucuxi global tests comparing Tucuxi with NONMEM.',
                        prog='tucuvalidation.py')

# For colorama, to reset the color on every new print
init(autoreset=True)

foldername = ''


def parse_the_args():
    """
    * This method parses the command line arguments. This project relies on windows having unix tools
      available in the console. We get this by installing git and allowing to use it via cmd.exe.
    """
    cli = 'tucucli'
    drgsfldr = os.path.abspath('drugfiles')

    parser.add_argument('-d', type=str, dest='drugspath',
                        help='Import folder paths for Tucuxi drugfiles (tdd)',
                        default=drgsfldr)
    parser.add_argument('-t', '--tucucli', type=str, dest='tucucli',
                        help='Command to execute Tucuxi cli, default is tucucli',
                        default=cli)
    return parser.parse_args()


def process_results(data):
    outputfilename = get_output_folder_name() + '/' + data[0]['queryId'] + '_results_adjustments.csv'
    outputfile = open(outputfilename, 'w')

    sep = ','

    s = ''
    s += 'drugModelId' + sep + 'dose' + sep + 'interval' + sep + 'infusion' + sep + 'score' + sep + \
         'target' + sep + 'PK parameters' + '\n'

    for patient in data:
        result = patient['results']['adjustment']
        if len(result) > 0:
            r = result[0]
            s += patient['drugModelId'] + sep + str(r[0]) + sep + str(r[1]) + sep + str(r[2]) + sep + \
                str(r[3]) + sep + str(r[4])
            for param_value in r[5]:
                s += sep + str(param_value)
            s += '\n'
        else:
            s += patient['drugModelId'] + sep + '-' + sep + '-' + sep + '-' + sep + '-' + sep + '-' + '\n'

    outputfile.write(s)
    outputfile.close()

    pltr = Plotter(get_output_folder_name(), 'study_tucuxi', '')

    pltr.plot_models(data, data[0]['queryId'] + '_models_comparison', 'Models comparison')


def process_queryfile(args, query_file: str, output_folder: str, id):

    tucucli = args.tucucli
    drug_path = args.drugspath
    output_file_path = output_folder + '/' + "query" + str(id + 1) + '.xml'

    print('')

    tucuclirun = TucuCliRun(tucucli, drug_path, output_file_path)
    tucuclirun.run_tucuxi_from_file(query_file)


def compare_query_responses(queries):
    once = True
    for i, test in enumerate(queries):
        for r, d, f in os.walk(test[1]):
            for file in f:
                if not filecmp.cmp(os.path.join(r, file), os.path.join(r, f[0]), shallow=False) and once:
                    print(Fore.RED + "Test{} : queries responses not equal".format(i + 1))
                    once = False
        once = True


def get_query_responses(args, queries):
    for test in queries:
        for id, queryfile in enumerate(test[0]):
            process_queryfile(args, queryfile, test[1], id)


def get_queries_path(query_responses):
    app_path = os.path.dirname(os.path.realpath(__file__))

    for r, d, f in os.walk(app_path):
        for i, dir in enumerate(d):
            for r1, d1, f1 in os.walk(os.path.join(r, dir)):
                for i1, dir1 in enumerate(d1):
                    if dir1 == "in_tqf":
                        query_responses.append([[]])
                        for r2, d2, f2 in os.walk(os.path.join(r1, dir1)):
                            for f, file in enumerate(f2):
                                query_responses[i][i1].append(os.path.join(r2, file))
                    else:
                        query_responses[i].append(os.path.join(r1, dir1))


def main():
    args = parse_the_args()

    queries = []

    get_queries_path(queries)

    get_query_responses(args, queries)

    compare_query_responses(queries)



    print(Fore.CYAN + 'End of script')


if __name__ == "__main__":
    main()
