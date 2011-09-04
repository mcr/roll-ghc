ghc:	ghc.c
	${CC} -O0 -g -o ghc ghc.c

check: ghc
	@cd tests && $(MAKE) check

