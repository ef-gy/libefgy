# don't delete intermediary files
.SECONDARY:

CFLAGS:=-O2
CXXFLAGS:=$(CFLAGS)
LDFLAGS:=

CC:=clang
CXX:=clang++

DATABASE:=
BINARIES:=$(basename $(notdir $(wildcard src/*.cpp)))

# meta rules
all: $(DATABASES) $(BINARIES)
clean:
	rm -f $(DATABASES) $(BINARIES); true
scrub: clean

# pattern rules for databases
%.sqlite3: src/%.sql
	rm -f $@ && $(SQLITE3) $@ < $<

# pattern rules for C++ code
%: src/%.cpp include/*/*.h
	$(CXX) -Iinclude/ $(CXXFLAGS) $< $(LDFLAGS) -o $@ && strip -x $@

test: cf
	./cf
