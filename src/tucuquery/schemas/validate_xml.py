#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Script to validate the tucucli xml exports against the computing_response.xsd
#
# File needed:  - computing_response.xsd
#
# Made by Andrea Enrile
#   20-09-2023
#
# Install packages:
#   pip install lxml

import os
from lxml import etree

class xmlFinder():
    def __init__(self, root_dir, xsd_file_path):
        self.xml_files = []
        self.xsd_path = xsd_file_path
        self.folder_parser(root_dir)


    def xml_file_finder(self, root_dir):
        for root, dirs, files in os.walk(root_dir):
            for d in dirs:
                self.xml_file_finder(d)
            for f in files:
                if f.lower().endswith(".xml"):
                    self.xml_files.append(os.path.join(root, f))

    def folder_parser(self, directory):
        for dirs in os.listdir(directory):
            self.xml_file_finder(dirs)

    def validator(self):
        logger = open("logs.log", "w")
        xsd_schema = etree.XMLSchema(etree.parse(self.xsd_path))
        for xml in self.xml_files:
            xml_file = etree.parse(xml)
            try:
                tmp = xsd_schema.assertValid(xml_file)
            except etree.DocumentInvalid:
                logger.write(f"{xml}\n")
                print(f"\x1b[38;2;255;25;25m{xml} is INVALID!!\x1b[0m")
                error = xsd_schema.error_log.last_error
                tmp = str(error).split(":")[1:]
                tmp = ":".join(tmp)
                print(f"\x1b[38;2;255;128;0m{tmp}\x1b[0m")
        logger.close()


directory = os.getcwd()

finder = xmlFinder(directory, os.path.dirname(__file__) + "/computing_response.xsd")

finder.validator()
