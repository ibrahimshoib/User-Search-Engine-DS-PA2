# --- Compiler & Flags ---
CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -Iheaders -Isolution

# --- Executables ---
RUNNER = test_runner

# --- Phony Targets ---
.PHONY: all clean run

# Default target: build runner and run it
all: run

# Rule to build the runner
$(RUNNER): test.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# Run target: build then execute
run: $(RUNNER)
	./$(RUNNER)

clean:
	rm -f $(RUNNER) tests/*_exe