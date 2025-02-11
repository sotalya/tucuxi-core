
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

cd $SCRIPTPATH/libs/botan
python3 configure.py --without-documentation
make -j 8 -f Makefile
