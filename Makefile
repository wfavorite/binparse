.PHONY: Gmake clean lean ccstart

CC=gcc
LD=gcc
CCOPTS=-Wall -Werror
LDOPTS=



# This is the stuff merged in (not yet tested)
bp: ccstart main.o bpfparse.o options.o strlib.o pmath.o penum.o
	@printf "Done.\nLinking..."
	@$(LD) $(LD_OPTS) -o bp main.o bpfparse.o options.o strlib.o pmath.o penum.o
	@printf "Done.\n"

# About the test target
#  - It is designed to build the test "framework" that is used to test some
#    of the more complex parsing functions. This is kind of important as they
#    are subject to some change during the code merge, and I want them to
#    stay functionally stable.
#  - I used it because I like ...status updates during a build rather than
#    compiler noize. (Your compile should be a silent affair - even with
#    warnings on.) But, more to the point, I was playing with gmake macros
#    and builtins. The $^ variable refers to all the items on the dependency
#    line. I was using that for the link-object list. But wait!!! $^ now
#    contains ccstart which is part of my pretty-print ascii output! So I 
#    moved the ccstart to an external target so that the non-object label
#    is not caught up in a list of objects.
#    [I am not inclined to use this variable expansion again. I think it is
#    overly terse and makes the build dependency harder to read. This bit of
#    craftiness cost me a cut-n-paste at the cost of a major loss in 
#    readability.
test: ccstart ldstart pmath penum
	@printf "Done.\n"

pmath: pmath_test.o pmath.o penum.o strlib.o
	@$(LD) $(LD_OPTS) -o pmath $^
	@printf "."

penum: penum_test.o penum.o pmath.o strlib.o
	@$(LD) $(LD_OPTS) -o penum $^
	@printf "."

pmath_test.o: pmath_test.c pmath.h penum.h
	@$(CC) $(CCOPTS) -c $<
	@printf "."

penum_test.o: penum_test.c pmath.h penum.h
	@$(CC) $(CCOPTS) -c $<
	@printf "."

pmath.o: pmath.c pmath.h strlib.h
	@$(CC) $(CCOPTS) -c $<
	@printf "."

penum.o: penum.c penum.h strlib.h
	@$(CC) $(CCOPTS) -c $<
	@printf "."

# STUB: Pull this from all code
#support.o: support.c support.h
#	@printf "  $< to $@\n"
#	@$(CC) $(CCOPTS) -c $<






main.o: main.c options.h bpfparse.h version.h strlib.h
	@$(CC) $(CC_OPTS) -c main.c
	@printf "."

bpfparse.o: bpfparse.c bpfparse.h strlib.h pmath.h penum.h
	@$(CC) $(CC_OPTS) -c bpfparse.c
	@printf "."

options.o: options.c options.h
	@$(CC) $(CC_OPTS) -c options.c
	@printf "."

strlib.o: strlib.c strlib.h
	@$(CC) $(CC_OPTS) -c strlib.c
	@printf "."




# Trial code
datapoint.o: datapoint.c
	$(CC) $(CC_OPTS) -c datapoint.c








ccstart:
	@printf "Compiling source files."

ldstart:
	@printf "Done.\n"
	@printf "Linking object files."



clean: lean
	@printf "Cleaning build directory."
	@rm -f pmath
	@printf "."
	@rm -f penum
	@printf "."
	@rm -f bp
	@printf ".Done.\n"

lean:
	@printf "Leaning build directory."
	@rm -f core
	@printf "."
	@rm -f *.o
	@printf "."
	@rm -f *~
	@printf "Done.\n"
