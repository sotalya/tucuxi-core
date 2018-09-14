# Author : Yann Thoma
# Institution : heig-vd
# Date : 2018.07.12
#
# This script validates every .xml file in the folder with the drug2.xsd schema

find -name "*.xml" -exec xmllint --schema drug2.xsd {} --noout \;
