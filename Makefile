all: testsymtablelist testsymtablehash

testsymtablelist: symtablelist.o testsymtable.o
	gcc217 symtablelist.o testsymtable.o -o testsymtablelist

testsymtablehash: symtablehash.o testsymtable.o
	gcc217 symtablehash.o testsymtable.o -o testsymtablehash

symtablelist.o: symtablelist.s symtable.h
	gcc217 -c symtablelist.s

symtablehash.o: symtablehash.s symtable.h
	gcc217 -c symtablehash.s

testsymtable.o: testsymtable.s symtable.h
	gcc217 -c testsymtable.s

symtablelist.s: symtablelist.i
	gcc217 -S symtablelist.i

symtablehash.s: symtablehash.i
	gcc217 -S symtablehash.i

testsymtable.c: testsymtable.i
	gcc217 -S testsymtable.i

symtablelist.i: symtablelist.c symtablelist.i
	gcc217 -E symtablelist.c > symtablelist.i

symtablehash.i: symtablehash.c symtablehash.i
	gcc217 -E symtablehash.c > symtablehash.i

testsymtable.i: testsymtable.c testsymtable.i
	gcc217 -E testsymtable.c > testsymtable.i