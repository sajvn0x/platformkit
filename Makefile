CC := clang++
CFLAGS := -Wall -O2
LINK_FLAGS := -lX11 -lXinerama -lXrandr
INCLUDE_FLAGS := -Isrc/
SOURCES := main.cc src/x11/x11_window.cc

run: compile
	@./main

compile:
	@${CC} $(CFLAGS) $(SOURCES) $(INCLUDE_FLAGS) $(LINK_FLAGS) -o main

fmt:
	@find src/ -iname '*.hh' -o -iname '*.cc' | xargs clang-format -i

