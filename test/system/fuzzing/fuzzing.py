import argparse
import logging
import os
import re
import shutil
import subprocess
import sys
import xml.etree.ElementTree as ET
from copy import copy
from cell_modifiers import CellModifier, E

TMP_TQF_DIR = "tmptqf"

index = 0
index_cfg = 0

# Logger configuration.
logging.basicConfig(level=logging.INFO)


def _run_one(args, suffix=""):
    """
    Run an individual execution of the Tucuxi CLI with the generated (fuzzed)
    config/input files.
    """
    cur_path = str(os.path.abspath(os.getcwd()))
    command = [
        os.path.join(cur_path, args.tucucli_path),
        "-d",
        os.path.join(cur_path, args.drug_files_dir),
        "-i",
        os.path.join(cur_path, args.input_file),
        "-o",
        os.path.join(cur_path, args.out_dir_path, suffix + ".xml"),
    ]
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    logging.debug("Command to execute: " + " ".join(command))

    stdout, stderr = process.communicate()

    if process.returncode == 0 or process.returncode == 1:
        if process.returncode == 1:
            with open(os.path.join(args.out_dir_path, args.logfile_name), "at") as f:
                f.write(f"Execution complete but some request failed at some stage of the process.\n")
        else:
            with open(os.path.join(args.out_dir_path, args.logfile_name), "at") as f:
                f.write(f"Execution complete!\n")

    elif process.returncode == 2 or process.returncode == 3:
        error = ""
        if process.returncode == 2:
            error = f"EXECUTION FAILED [{process.returncode}]: No request could be fully processed\n"
        else:
            error = f"EXECUTION FAILED [{process.returncode}]: The query file could not be loaded.\n"

        with open(os.path.join(args.out_dir_path, args.logfile_name), "at") as f:
            f.write(
                error
            )

        with open(os.path.join(args.out_dir_path, args.error_logfile), "at") as f:
            f.write(
                error
            )

    elif stderr:
        error = f"EXECUTION FAILED [{process.returncode}]: Programm crashed during processing of file: {suffix}\n"
        with open(os.path.join(args.out_dir_path, args.logfile_name), "at") as f:
            f.write(
                error
            )

        with open(os.path.join(args.out_dir_path, args.error_logfile), "at") as f:
            f.write(
                error
            )
        with open(os.path.join(args.out_dir_path, args.crash_logfile), "at") as f:
            f.write(
                error
            )
            f.write(stderr.decode() + "\n")

    else:
        error = f"EXECUTION FAILED [{process.returncode}]: Unknown error during processing of file: {suffix}\n"
        with open(os.path.join(args.out_dir_path, args.logfile_name), "at") as f:
            f.write(
                error
            )

        with open(os.path.join(args.out_dir_path, args.error_logfile), "at") as f:
            f.write(
                error
            )

def process_test(args, value_modifier, file_to_modify, file_prefix, attr_to_update):
    """
    Alter the given configuration file/input file and execute the Tucuxi CLI.
    """
    tree = ET.parse(file_to_modify)
    root = tree.getroot()

    def process_element(element):
        for child in element:
            if child.text is not None and child.text.strip() != "":
                try:
                    original = copy(child.text)
                    child.text = str(value_modifier(child.tag, child.text))
                    global index
                    index += 1
                    modified_file = os.path.join(
                        TMP_TQF_DIR, f"{file_prefix}_{index}.tqf"
                    )
                    tree.write(modified_file, encoding="utf-8", xml_declaration=True)

                    with open(
                        os.path.join(args.out_dir_path, args.logfile_name), "at"
                    ) as f:
                        f.write(f"File modified: {modified_file}\n")
                        f.write(f"Changed cell {child.tag} to {child.text}\n")

                    setattr(args, attr_to_update, modified_file)

                    suffix = f"{file_prefix}_{index}"
                    _run_one(args, suffix)

                    with open(
                        os.path.join(args.out_dir_path, args.logfile_name), "at"
                    ) as f:
                        f.write("---------------\n")

                    child.text = str(original)
                except (ValueError, TypeError, E):
                    pass

            process_element(child)

    process_element(root)


def test_input(args, value_modifier):
    process_test(args, value_modifier, args.original_input, "imatinib", "input_file")


def parse_arguments():
    """
    Input arguments parsing.
    """
    parser = argparse.ArgumentParser(
        description="Configure global script variables.", add_help=True
    )

    parser.add_argument(
        "-i",
        "--original_input",
        type=str,
        default="imatinib.tqf",
        help="Path of the original input TQF (default: 'imatinib.tqf')",
    )
    parser.add_argument(
        "-o",
        "--out_dir_path",
        type=str,
        default="output",
        help="Path to the output directory (default: 'output')",
    )
    parser.add_argument(
        "-l",
        "--logfile_name",
        type=str,
        default="000_LOG.txt",
        help="Name of the log file (default: '000_LOG.txt')",
    )
    parser.add_argument(
        "-c",
        "--tucucli_path",
        type=str,
        default="../../../src/tucucli/build/tucucli",
        help="Path to the Tucuxi CLI executable (default: '../../../src/tucucli/build/tucucli')",
    )
    parser.add_argument(
        "-d",
        "--drug_files_dir",
        type=str,
        default="../../../../tucuxi-drugs/drugfiles/",
        help="Path to the drug files directory (default: '../../../../tucuxi-drugs/drugfiles/')",
    )

    return parser.parse_args()


def get_unique_logfile_name(args):
    base_name, ext = os.path.splitext(args.logfile_name)

    counter = int(base_name.split("_")[0])
    prefix = "_".join(base_name.split("_")[1:])

    while os.path.exists(
        os.path.join(args.out_dir_path, f"{counter:03d}_{prefix}{ext}")
    ):
        counter += 1

    args.logfile_name = f"{counter:03d}_{prefix}{ext}"
    args.error_logfile = f"{counter:03d}_ERROR{ext}"
    args.crash_logfile = f"{counter:03d}_CRASH{ext}"


def valid_paths(args) -> bool:
    """
    Check that the given paths are valid/existing ones.

    Parameters
    ----------
    args: object carrying the paths given as parameters

    Returns
    -------
    True if all the given paths are valid/existing, False otherwise.
    The in
    """
    paths_valid = True

    if not os.path.isfile(args.original_input):
        logging.error("Invalid input TQF file: " + args.original_input)
        paths_valid = False
    if not os.path.isfile(args.tucucli_path):
        logging.error("Invalid Tucuxi CLI executable path: " + args.tucucli_path)
        paths_valid = False
    if not os.path.isdir(args.drug_files_dir):
        logging.error("Invalid drug files directory path: " + args.drug_files_dir)
        paths_valid = False

    return paths_valid


if __name__ == "__main__":
    args = parse_arguments()

    # Create the output directory (if missing).
    if not os.path.isdir(args.out_dir_path):
        os.makedirs(args.out_dir_path, exist_ok=False)
    get_unique_logfile_name(args)

    logging.info(f"INPUT_FILE: {args.original_input}")
    logging.info(f"OUT_DIR_PATH: {args.out_dir_path}")
    logging.info(f"LOGFILE_NAME: {args.logfile_name}")
    logging.info(f"tucucli_PATH: {args.tucucli_path}")
    logging.info(f"DRUG_FILES_DIR: {args.drug_files_dir}")

    if not valid_paths(args):
        sys.exit(-1)

    # Create an empty tmp directory for TQF files.
    if os.path.isdir(TMP_TQF_DIR):
        shutil.rmtree(TMP_TQF_DIR)
    os.makedirs(TMP_TQF_DIR, exist_ok=False)

    # Retrieve the set of functions used to fuzz XML values.
    modifiers = [
        getattr(CellModifier, func)
        for func in dir(CellModifier)
        if callable(getattr(CellModifier, func)) and not func.startswith("__")
    ]

    cnt = 1
    for modifier in modifiers:
        modifier_str = str(modifier).split(".")[-1].split(" ")[0]
        logging.info(
            "## Test "
            + str(cnt)
            + " / "
            + str(len(modifiers))
            + " --- ["
            + modifier_str
            + "]"
        )
        test_input(args, modifier)
        # Reset the input_file for configuration 
        #   if not reset test will be done with last modified input file.
        cnt += 1
