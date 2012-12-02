CC := g++
BINNAME := output
INPUTFILES := main.cpp Player.cpp Timer.cpp Enemy.cpp

# Stuff we need
INCLUDECFLAGS := -L/usr/lib `pkg-config --libs --cflags sdl` -lSDL_image -lSDL_mixer
INCLUDELIBFLAGS := -I"." -lm #-lreadline # -lm `pkg-config --libs sdl`
INCLUDEFLAGS := $(INCLUDECFLAGS) $(INCLUDELIBFLAGS)

# Flags in common by all
CFLAGS := -std=c++11 -Wall -W -Wextra -pedantic -pedantic-errors -Wfloat-equal -Wundef -Wshadow -Winit-self -Winline -Wpointer-arith -Wcast-align


# Flags for debugging builds
CDFLAGS := $(CFLAGS) -g -O3 -fstack-protector-all -Wstack-protector -Wstrict-overflow=4
# Flags for normal builds
CNFLAGS := $(CFLAGS) -O3 -fno-stack-protector -s
# Flags for very aggressive builds
COFLAGS := $(CFLAGS) -O3 -ffast-math -funsafe-loop-optimizations -Wunsafe-loop-optimizations \
-fstrict-aliasing -Wstrict-aliasing -fstrict-overflow

SSEFLAGS := -msse2 -mfpmath=sse

# This stuff is kinda extreme...
EXTREMEFLAGS := -funsafe-math-optimizations -ffinite-math-only -funroll-all-loops

# Profiling stuff:
# -fbranch-probabilities (look more into that kind of stuff) -fpeel-loops -fprofile-use

# Other interesting stuff to look into:
# -floop-block -floop-interchange -floop-strip-mine -fprefetch-loop-arrays


all: default
default: clean
	$(CC) $(CNFLAGS) $(INCLUDEFLAGS) $(INPUTFILES) -o $(BINNAME) -lm
default-sse: clean
	@$(CC) $(SSEFLAGS) $(CNFLAGS) $(INCLUDEFLAGS) $(INPUTFILES) -o $(BINNAME)
debug: clean
	@$(CC) $(CDFLAGS) $(INCLUDEFLAGS) $(INPUTFILES) -o $(BINNAME)
optimized: clean
	@$(CC) $(COFLAGS) $(INCLUDEFLAGS) $(INPUTFILES) -o $(BINNAME)
optimized-sse: clean
	@$(CC) $(SSEFLAGS) $(COFLAGS) $(INCLUDEFLAGS) $(INPUTFILES) -o $(BINNAME)
extremely-optimized: clean
	@$(CC) $(COFLAGS) $(EXTREMEFLAGS) $(INCLUDEFLAGS) $(INPUTFILES) -o $(BINNAME)
run:
	@time ./$(BINNAME)
clean:
	@$(RM) $(BINNAME)
	@$(RM) *.plist
