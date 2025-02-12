
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

cd $SCRIPTPATH/libs/botan
if [[ $OSTYPE == 'darwin'* ]]; then
	python3 configure.py --without-documentation
	make -j 8 -f Makefile
else
	python configure.py --without-documentation
	make -j$(nproc) -f Makefile
fi
