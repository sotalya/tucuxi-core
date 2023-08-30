mkdir qtcreator/build
mkdir qtcreator/build/coverage
mkdir qtcreator/build/coverage/tucucore
cd qtcreator/build/coverage/tucucore
qmake ../../../tucutestcore/tucutestcore.pro CONFIG+=config_coverage CONFIG+=debug
make -j6
./tucutestcore
lcov --no-external --capture --directory . --base-directory ../../../../../src/ --output-file report.info
cd ../../../..

mkdir qtcreator/build/coverage/tucucommon
cd qtcreator/build/coverage/tucucommon
qmake ../../../tucutestcommon/tucutestcommon.pro CONFIG+=config_coverage CONFIG+=debug
make -j6
./tucutestcommon
lcov --no-external --capture --directory . --base-directory ../../../../../src/ --output-file report.info
cd ../../../..

mkdir qtcreator/build/coverage/tucuquery
cd qtcreator/build/coverage/tucuquery
qmake ../../../tucutestquery/tucutestquery.pro CONFIG+=config_coverage CONFIG+=debug
make -j6
./tucutestquery
lcov --no-external --capture --directory . --base-directory ../../../../../src/ --output-file report.info
cd ../../../..

cd qtcreator/build/coverage
lcov --add-tracefile tucucore/report.info --add-tracefile tucucommon/report.info --add-tracefile tucuquery/report.info --output-file total.info

genhtml total.info --output-directory=./html

