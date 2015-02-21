DESTDIR:=
PREFIX:=/usr/local
BINDIR:=$(DESTDIR)$(PREFIX)/bin
INCLUDEDIR:=$(DESTDIR)$(PREFIX)/include
MANDIR:=$(DESTDIR)$(PREFIX)/share/man

# define these in your project!
NAME:=
BASE=$(NAME)
VERSION:=1

CC:=clang
CXX:=clang++
EMXX:=em++
PKGCONFIG:=pkg-config
INSTALL:=install
XSLTPROC:=xsltproc
CURL:=curl
CLANG_FORMAT:=clang-format

LIBRARIES:=

DEBUG:=false

PCCFLAGS:=$(shell $(PKGCONFIG) --cflags $(LIBRARIES) 2>/dev/null)
PCLDFLAGS:=$(shell $(PKGCONFIG) --libs $(LIBRARIES) 2>/dev/null)
CFLAGS:=-O2 $(shell if $(DEBUG); then echo '-g'; fi)
CXXFLAGS:=$(CFLAGS)
EMCFLAGS:=-O2 --llvm-lto 3
EMXXFLAGS:=$(EMCFLAGS)
LDFLAGS:=
CXX_STANDARD:=c++11

DATABASE:=
BINARIES:=$(basename $(notdir $(wildcard src/*.cpp)) $(addprefix test-case-,$(notdir $(wildcard src/test-case/*.cpp))))
JSBINARIES:=$(addsuffix .js,$(BINARIES))
TESTBINARIES:=$(filter test-case-%,$(BINARIES))

IGNOREBINARIES:=
IBINARIES:=$(addprefix $(BINDIR)/,$(filter-out $(IGNOREBINARIES) test-case-%,$(BINARIES)))
IINCLUDES:=$(addprefix $(INCLUDEDIR)/$(BASE)/,$(notdir $(wildcard include/$(BASE)/*.h)))
IMANPAGES:=$(addprefix $(MANDIR)/man1/,$(notdir $(wildcard src/*.1)))

DATAHEADERS=$(wildcard include/data/*.h)

# don't delete intermediary files
.SECONDARY:

# meta rules
all: $(DATABASES) $(BINARIES)
clean:
	rm -f $(DATABASES) $(BINARIES); true
scrub: clean

install: $(IBINARIES) $(IINCLUDES) $(IMANPAGES)
uninstall:
	rm -f $(IBINARIES) $(IINCLUDES) $(IMANPAGES)

archive: ../$(NAME)-$(VERSION).tar.gz

../$(NAME)-$(VERSION).tar.gz:
	git archive --format=tar --prefix=$(NAME)-$(VERSION)/ HEAD | gzip -9 >$@

# meta rules for documentation
documentation documentation/xml/combine.xslt documentation/xml/index.xml: doxyfile $(wildcard include/*/* xslt/doxy*)
	doxygen $<

documentation.xml: documentation/xml/combine.xslt documentation/xml/index.xml xslt/doxygen*
	xsltproc documentation/xml/combine.xslt documentation/xml/index.xml | xsltproc xslt/doxygen-post-process.xslt - > $@

# meta rules to reformat sources
format:
	$(CLANG_FORMAT) -i $(wildcard src/*.cpp) $(wildcard src/*/*.cpp) $(IINCLUDES)

# meta rules for javascript
js: $(JSBINARIES)

#run unit tests
test: $(addprefix run-,$(TESTBINARIES))

run-test-case-%: test-case-%
	@echo TEST BATCH: $*
	@./$^
	@echo PASSED

# pattern rules to install things
$(BINDIR)/%: %
	$(INSTALL) -D $< $@

$(INCLUDEDIR)/%: include/%
	$(INSTALL) -D $< $@

$(MANDIR)/man1/%.1: src/%.1
	$(INSTALL) -D $< $@

# pattern rules for databases
%.sqlite3: src/%.sql
	rm -f $@ && $(SQLITE3) $@ < $<

# pattern rules for C++ code
%: src/%.cpp include/*/*.h $(DATAHEADERS)
	$(CXX) -std=$(CXX_STANDARD) -Iinclude/ $(CXXFLAGS) $(PCCFLAGS) $< $(LDFLAGS) $(PCLDFLAGS) -o $@ && ($(DEBUG) || strip -x $@)

test-case-%: src/test-case/%.cpp include/*/*.h
	$(CXX) -std=$(CXX_STANDARD) -Iinclude/ -DRUN_TEST_CASES $(CXXFLAGS) $(PCCFLAGS) $< $(LDFLAGS) $(PCLDFLAGS) -o $@

%.js: src/%.cpp include/*/*.h
	$(EMXX) -std=$(CXX_STANDARD) -Iinclude/ -D NOLIBRARIES $(EMXXFLAGS) $< $(LDFLAGS) -o $@
