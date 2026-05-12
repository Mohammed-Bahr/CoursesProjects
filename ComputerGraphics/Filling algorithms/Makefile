# ──────────────────────────────────────────────────────────────────────────
#  Makefile — Scanline AEL Polygon Fill (SDL2)
#  Works on Ubuntu and Fedora/RHEL family.
#
#  Usage:
#    make            # build
#    make run        # build + run (animated mode, default)
#    make static     # build + run in static (instant fill) mode
#    make clean      # remove binary
# ──────────────────────────────────────────────────────────────────────────

CXX      := g++
TARGET   := scanline_ael
SRC      := main.cpp

# SDL2 flags via pkg-config (works on Ubuntu & Fedora after install)
SDL2_CFLAGS := $(shell pkg-config --cflags sdl2)
SDL2_LIBS   := $(shell pkg-config --libs   sdl2)

CXXFLAGS := -std=c++17 -O2 -Wall -Wextra $(SDL2_CFLAGS)
LDFLAGS  := $(SDL2_LIBS)

.PHONY: all run static clean install-deps

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@
	@echo "✓  Built: $(TARGET)"

run: all
	./$(TARGET)

static: all
	./$(TARGET) --static

clean:
	rm -f $(TARGET)

# ── Dependency installation helpers ───────────────────────────────────────
install-deps-ubuntu:
	sudo apt-get update && sudo apt-get install -y libsdl2-dev

install-deps-fedora:
	sudo dnf install -y SDL2-devel
