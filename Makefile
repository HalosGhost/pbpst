PROGNM =  pbpst
PREFIX ?= /usr/local
DOCDIR ?= $(DESTDIR)$(PREFIX)/share/man
LIBDIR ?= $(DESTDIR)$(PREFIX)/lib
BINDIR ?= $(DESTDIR)$(PREFIX)/bin
ZSHDIR ?= $(DESTDIR)$(PREFIX)/share/zsh
BSHDIR ?= $(DESTDIR)$(PREFIX)/share/bash-completions

.PHONY: all clean cov-build install uninstall

all:
	@mkdir -p ./dist
	@tup upd

clean:
	@rm -rf -- dist cov-int $(PROGNM).tgz make.sh

cov-build: clean
	@tup generate make.sh
	@mkdir -p ./dist
	@cov-build --dir cov-int ./make.sh
	@tar czvf $(PROGNM).tgz cov-int

install:
	@install -Dm755 dist/$(PROGNM)   $(BINDIR)/$(PROGNM)
	@install -Dm644 dist/$(PROGNM).1 $(DOCDIR)/man1/$(PROGNM).1
	@install -Dm644 dist/pbpst_db.5  $(DOCDIR)/man5/pbpst_db.5
	@install -Dm644 dist/zsh         $(ZSHDIR)/site-functions/_$(PROGNM)
	@install -Dm644 dist/bash        $(BSHDIR)/completions/$(PROGNM)

uninstall:
	@rm -f $(BINDIR)/$(PROGNM)
	@rm -f $(DOCDIR)/man1/$(PROGNM).1
	@rm -f $(DOCDIR)/man5/pbpst_db.5
	@rm -f $(ZSHDIR)/site-functions/_$(PROGNM)
	@rm -f $(BSHDIR)/completions/$(PROGNM)

include Makeeaster
