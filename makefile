DESTDIR:=
PREFIX:=/usr/local
BINDIR:=$(DESTDIR)$(PREFIX)/bin
INCLUDEDIR:=$(DESTDIR)$(PREFIX)/include
MANDIR:=$(DESTDIR)$(PREFIX)/share/man

NAME:=libefgy
VERSION:=3

CC:=clang
CXX:=clang++
EMXX:=em++
PKGCONFIG:=pkg-config
INSTALL:=install
XSLTPROC:=xsltproc

LIBRARIES:=

DEBUG:=false

PCCFLAGS:=$(shell $(PKGCONFIG) --cflags $(LIBRARIES) 2>/dev/null)
PCLDFLAGS:=$(shell $(PKGCONFIG) --libs $(LIBRARIES) 2>/dev/null)
CFLAGS:=-O2 $(shell if $(DEBUG); then echo '-g'; fi)
CXXFLAGS:=$(CFLAGS)
EMCFLAGS:=-O2 --llvm-lto 3
EMXXFLAGS:=$(EMCFLAGS)
LDFLAGS:=

DATABASE:=
BINARIES:=$(basename $(notdir $(wildcard src/*.cpp)))
JSBINARIES:=$(addsuffix .js,$(BINARIES))
TESTBINARIES:=$(filter test-%,$(BINARIES))

IGNOREBINARIES:=
IBINARIES:=$(addprefix $(BINDIR)/,$(filter-out $(IGNOREBINARIES) test-%,$(BINARIES)))
IINCLUDES:=$(addprefix $(INCLUDEDIR)/ef.gy/,$(notdir $(wildcard include/ef.gy/*.h)))
IMANPAGES:=$(addprefix $(MANDIR)/man1/,$(notdir $(wildcard src/*.1)))

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
documentation.xml: doxyfile include/*/* xslt/doxy*
	doxygen $^
	cd documentation/xml && $(XSLTPROC) combine.xslt index.xml | $(XSLTPROC) ../../xslt/doxygen-post-process.xslt - > ../../$@

# meta rules for javascript
js: $(JSBINARIES)

#run unit tests
test: $(TESTBINARIES)
	for i in $^; do echo TEST: $$i; ./$$i; done

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
%: src/%.cpp include/*/*.h
	$(CXX) -std=c++0x -Iinclude/ $(CXXFLAGS) $(PCCFLAGS) $< $(LDFLAGS) $(PCLDFLAGS) -o $@ && ($(DEBUG) || strip -x $@)

%.js: src/%.cpp include/*/*.h
	$(EMXX) -std=c++0x -Iinclude/ -D NOLIBRARIES $(EMXXFLAGS) $< $(LDFLAGS) -o $@
