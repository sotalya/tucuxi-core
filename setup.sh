
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

cd $SCRIPTPATH/libs/botan
python configure.py --without-documentation
make -j$(nproc) -f Makefile

