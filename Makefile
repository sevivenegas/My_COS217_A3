all: testsymtablelist testsymtablehash

testsymtablelist: symtablelist.o testsymtable.o
	gcc217 symtablelist.o testsymtable.o -o testsymtablelist

testsymtablehash: symtablehash.o testsymtable.o
	gcc217 symtablehash.o testsymtable.o -o testsymtablehash

symtablelist.o: symtablelist.c symtable.h
	gcc217 -c symtablelist.c

symtablehash.o: symtablehash.c symtable.h
	gcc217 -c symtablehash.c

testsymtable.o: testsymtable.c symtable.h
	gcc217 -c testsymtable.c