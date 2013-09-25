root:=http://colouri.se/
name:=Magnus Achim Deininger

# programmes
ZIP:=zip

# don't delete intermediary files
.SECONDARY:

# meta rules
all: $(DATABASES) cf
clean:
	rm -f $(DATABASES) cf; true
scrub: clean

# pattern rules for databases
%.sqlite3: src/%.sql
	rm -f $@ && $(SQLITE3) $@ < $<

%: src/%.cpp include/ef.gy/*.h
	clang++ -Iinclude/ -O2 $< -o $@ && strip -x $@

test: cf
	./cf
