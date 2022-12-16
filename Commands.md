Compile all:
make -f Makefile.assets -j8

Compile Seperate:

make

make -f Makefile.tools

make -f Makefile.tim -j8

make -f Makefile.vag -j8

make -f Makefile.xa -j8

make -f Makefile.cht -j8

mkpsxiso -y funkin.xml