.PHONY: Gmake clean lean ccstart

CC=gcc
LD=gcc
CCOPTS=-Wall -Werror
LDOPTS=

# This target was used so that $^ did not expand to "ccstart ..."
# This looks like the end of that kind of macro expansion.
test: ccstart pmath penum

pmath: pmath_test.o pmath.o penum.o support.o
	@printf "Linking object files.\n"
	@$(LD) $(LD_OPTS) -o pmath $^

penum: penum_test.o penum.o pmath.o support.o
	@printf "Linking object files.\n"
	@$(LD) $(LD_OPTS) -o penum $^

pmath_test.o: pmath_test.c pmath.h penum.h
	@printf "  $< to $@\n"
	@$(CC) $(CCOPTS) -c $<

penum_test.o: penum_test.c pmath.h penum.h
	@printf "  $< to $@\n"
	@$(CC) $(CCOPTS) -c $<

pmath.o: pmath.c pmath.h support.h
	@printf "  $< to $@\n"
	@$(CC) $(CCOPTS) -c $<

penum.o: penum.c penum.h support.h
	@printf "  $< to $@\n"
	@$(CC) $(CCOPTS) -c $<

support.o: support.c support.h
	@printf "  $< to $@\n"
	@$(CC) $(CCOPTS) -c $<

ccstart:
	@printf "Compiling source files.\n"


clean: lean
	@printf "Cleaning build directory."
	@rm -f pmath
	@printf "."
	@rm -f penum
	@printf ".Done.\n"

lean:
	@printf "Leaning build directory."
	@rm -f core
	@printf "."
	@rm -f *.o
	@printf "."
	@rm -f *~
	@printf "Done.\n"
