PROGNM  =  pbpst
PREFIX  ?= /usr/local
DOCDIR  ?= $(DESTDIR)$(PREFIX)/share/man
LIBDIR  ?= $(DESTDIR)$(PREFIX)/lib
BINDIR  ?= $(DESTDIR)$(PREFIX)/bin
ZSHDIR  ?= $(DESTDIR)$(PREFIX)/share/zsh
BASHDIR ?= $(DESTDIR)$(PREFIX)/share/bash-completion
LOCDIR  ?= $(DESTDIR)$(PREFIX)/share/locale

.PHONY: all clean gen complexity clang-analyze cov-build pot simple install uninstall

all: dist
	@tup upd

clean:
	@rm -rf -- dist cov-int $(PROGNM).tgz make.sh ./src/*.plist \
		./i18n/$(PROGNM).pot

dist:
	@mkdir -p ./dist/locale

gen: clean
	@tup generate make.sh

complexity:
	@complexity -h ./src/*

cov-build: gen dist
	@cov-build --dir cov-int ./make.sh
	@tar czvf $(PROGNM).tgz cov-int

clang-analyze:
	@(pushd ./src; clang-check -analyze ./*.c)

pot:
	@xgettext -k_ -c -d $(PROGNM) --no-wrap -o i18n/$(PROGNM).pot ./src/*.{c,h}

simple: gen dist
	@./make.sh

install:
	@install -Dm755 dist/$(PROGNM)   $(BINDIR)/$(PROGNM)
	@install -Dm644 dist/$(PROGNM).1 $(DOCDIR)/man1/$(PROGNM).1
	@install -Dm644 dist/pbpst_db.5  $(DOCDIR)/man5/pbpst_db.5
	@install -Dm644 dist/zsh         $(ZSHDIR)/site-functions/_$(PROGNM)
	@install -Dm644 dist/bash        $(BASHDIR)/completions/$(PROGNM)
	@(for i in dist/locale/*.mo; do \
		int="$${i/.mo/}"; int="$${int#dist/locale/}"; \
		install -Dm644 $$i "$(LOCDIR)/$$int/LC_MESSAGES/$(PROGNM).mo"; \
	done)

uninstall:
	@rm -f $(BINDIR)/$(PROGNM)
	@rm -f $(DOCDIR)/man1/$(PROGNM).1
	@rm -f $(DOCDIR)/man5/pbpst_db.5
	@rm -f $(ZSHDIR)/site-functions/_$(PROGNM)
	@rm -f $(BASHDIR)/completions/$(PROGNM)
	@rm -f $(LOCDIR)/*/LC_MESSAGES/$(PROGNM).mo

include Makeeaster
