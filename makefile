# Makefile

# The 'all' target builds the 'linker' executable
all: linker

# Compiling the 'linker' executable from 'linker.cpp'
linker: linker.cpp
	g++ -g linker.cpp -o linker

# 'clean' target for removing the compiled binary and editor backup files
clean:
	rm -f linker *~
