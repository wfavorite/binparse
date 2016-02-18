.PHONY: Gmake clean lean ccstart

# The default options
CC=gcc
LD=gcc
CC_OPTS=-Wall -Werror -m64 -O2 -DPORT_$(shell uname -s)
LD_OPTS=-m64 -O2
OSSUP=GCC
CCSUP_STOP=true

ifeq "$(shell uname -s)" "Linux"
        CC=gcc
        CC_OPTS=-Wall -Werror -m64 -O2 -DPORT_$(shell uname -s)
        LD_OPTS=-m64 -O2
        OSSUP=GCC
        LANG=C
endif

ifeq "$(shell uname -s)" "SunOS"
        CC=/bb/util/common/SS12_3-20131030/SUNWspro/bin/c99
        LD=/bb/util/common/SS12_3-20131030/SUNWspro/bin/c99
        CC_OPTS=-xtarget=generic -m64 -xO3 -DPORT_$(shell uname -s) -D_XOPEN_SOURCE=600 -D__EXTENSIONS__
        LD_OPTS=-m64 -xO3
        OSSUP=Sun Studio
        LANG=C
endif

ifeq "$(shell uname -s)" "AIX"
        CC=/bb/util/version11-042012/usr/vac/bin/xlc
        LD=/bb/util/version11-042012/usr/vac/bin/xlc
        CC_OPTS=-q64 -O2 -DPORT_$(shell uname -s)
        LD_OPTS=-q64 -O2
        OSSUP=xlc
endif


ifneq "$(shell ls -d /bb/bin)" "/bb/bin"
        CC=gcc
        LD=gcc
        CC_OPTS=-Wall -Werror -m64 -O2 -DPORT_$(shell uname -s)
        LD_OPTS=-m64 -O2
        OSSUP=GCC
endif


bp: ccstart main.o bpfparse.o options.o strlib.o pmath.o penum.o slfile.o bpdata.o binpass.o display.o eswap.o
	@printf "Done.\nLinking..."
	@$(LD) $(LD_OPTS) -o bp main.o bpfparse.o options.o strlib.o pmath.o penum.o slfile.o bpdata.o binpass.o display.o eswap.o
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

pmath: pmath_test.o pmath.o penum.o strlib.o slfile.o
	@$(LD) $(LD_OPTS) -o pmath $^
	@printf "."

penum: penum_test.o penum.o pmath.o strlib.o slfile.o
	@$(LD) $(LD_OPTS) -o penum $^
	@printf "."

pmath_test.o: pmath_test.c pmath.h penum.h
	@$(CC) $(CC_OPTS) -c $<
	@printf "."

penum_test.o: penum_test.c pmath.h penum.h
	@$(CC) $(CC_OPTS) -c $<
	@printf "."

pmath.o: pmath.c pmath.h strlib.h
	@$(CC) $(CC_OPTS) -c $<
	@printf "."

penum.o: penum.c penum.h bpdata.h strlib.h
	@$(CC) $(CC_OPTS) -c $<
	@printf "."

slfile.o: slfile.c slfile.h strlib.h
	@$(CC) $(CC_OPTS) -c $<
	@printf "."

bpdata.o: bpdata.c bpdata.h
	@$(CC) $(CC_OPTS) -c $<
	@printf "."

main.o: main.c options.h bpfparse.h version.h strlib.h binpass.h bpdata.h display.h
	@$(CC) $(CC_OPTS) -c $<
	@printf "."

bpfparse.o: bpfparse.c bpfparse.h strlib.h slfile.h pmath.h penum.h bpdata.h
	@$(CC) $(CC_OPTS) -c $<
	@printf "."

options.o: options.c options.h
	@$(CC) $(CC_OPTS) -c $<
	@printf "."

strlib.o: strlib.c strlib.h
	@$(CC) $(CC_OPTS) -c $<
	@printf "."

eswap.o: eswap.c eswap.h
	@$(CC) $(CC_OPTS) -c $<
	@printf "."

binpass.o: binpass.c binpass.h options.h bpdata.h
	@$(CC) $(CC_OPTS) -c $<
	@printf "."

display.o: display.c display.h bpdata.h options.h
	@$(CC) $(CC_OPTS) -c $<
	@printf "."

# PHONY targets
ccstart: ccsup
	@printf "Compiling source files."

ldstart:
	@printf "Done.\n"
	@printf "Linking object files."

ccsup:
	@printf "The compiler used: %s\n" "$(OSSUP)"
	@$(CCSUP_STOP)

Gmake:
	@printf "ERROR: This is a gmake makefile. Use gmake, not make." >&2
	@false

# Cleanup related PHONYs
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
