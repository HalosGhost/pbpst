PROGNM  =  pbpst
PREFIX  ?= /usr/local
DOCDIR  ?= $(DESTDIR)$(PREFIX)/share/man
LIBDIR  ?= $(DESTDIR)$(PREFIX)/lib
BINDIR  ?= $(DESTDIR)$(PREFIX)/bin
ZSHDIR  ?= $(DESTDIR)$(PREFIX)/share/zsh
BASHDIR ?= $(DESTDIR)$(PREFIX)/share/bash-completion

.PHONY: all clean gen clang-analyze cov-build pot simple install uninstall

all: dist
	@tup upd

clean:
	@rm -rf -- dist cov-int $(PROGNM).tgz make.sh ./src/*.plist ./po/$(PROGNM).pot

dist:
	@mkdir -p ./dist

gen: clean
	@tup generate make.sh

cov-build: gen dist
	@cov-build --dir cov-int ./make.sh
	@tar czvf $(PROGNM).tgz cov-int

clang-analyze:
	@(pushd ./src; clang-check -analyze ./*.c)

pot: clean
	@xgettext -k_ -d $(PROGNM) -o po/$(PROGNM).pot ./src/*.c

simple: gen dist
	@./make.sh

install:
	@install -Dm755 dist/$(PROGNM)   $(BINDIR)/$(PROGNM)
	@install -Dm644 dist/$(PROGNM).1 $(DOCDIR)/man1/$(PROGNM).1
	@install -Dm644 dist/pbpst_db.5  $(DOCDIR)/man5/pbpst_db.5
	@install -Dm644 dist/zsh         $(ZSHDIR)/site-functions/_$(PROGNM)
	@install -Dm644 dist/bash        $(BASHDIR)/completions/$(PROGNM)

uninstall:
	@rm -f $(BINDIR)/$(PROGNM)
	@rm -f $(DOCDIR)/man1/$(PROGNM).1
	@rm -f $(DOCDIR)/man5/pbpst_db.5
	@rm -f $(ZSHDIR)/site-functions/_$(PROGNM)
	@rm -f $(BASHDIR)/completions/$(PROGNM)

include Makeeaster
