PROGNM  =  pbpst
PREFIX  ?= /usr/local
DOCDIR  ?= $(DESTDIR)$(PREFIX)/share/man
LIBDIR  ?= $(DESTDIR)$(PREFIX)/lib
BINDIR  ?= $(DESTDIR)$(PREFIX)/bin
ZSHDIR  ?= $(DESTDIR)$(PREFIX)/share/zsh
BASHDIR ?= $(DESTDIR)$(PREFIX)/share/bash-completion
LOCDIR  ?= $(DESTDIR)$(PREFIX)/share/locale

CC = clang
CFLAGS = -g -O3 -fPIE -pie -Weverything -Werror -Wno-disabled-macro-expansion -std=c11 -ggdb -D_FORTIFY_SOURCE=2 -fstack-protector-strong -march=native --param=ssp-buffer-size=1 -Wl,-z,relro,-z,now -flto -fsanitize=undefined -fsanitize-trap=undefined
LDFLAGS = `pkg-config --libs-only-l libcurl jansson`
SOURCES = main.c callback.c pb.c pbpst_db.c usage.c
VER = `git describe --long --tags`

.PHONY: all bin clean complexity clang-analyze cmp cov-build doc i18n pot install uninstall

all: dist bin cmp doc i18n

clang-analyze:
	@(cd ./src; clang-check -analyze ./*.c)

clean:
	@rm -rf -- dist cov-int $(PROGNM).tgz make.sh ./src/*.plist \
		./i18n/$(PROGNM).pot

complexity:
	@complexity -h ./src/*

cov-build: clean dist
	@cov-build --dir cov-int ./make.sh
	@tar czvf $(PROGNM).tgz cov-int

cmp: dist
	@(for i in cmp/*sh*; do \
		cp "$$i" dist/; \
	done);

bin: dist
	@(cd src; \
		$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o ../dist/pbpst \
	)

dist:
	@mkdir -p dist/locale

doc: dist
	@(cd doc; \
		sphinx-build -b man -Dversion=$(VER) \
					-d doctree -E . ../dist $(PROGNM).rst $(PROGNM)_db.rst; \
		rm -r -- doctree; \
	)

i18n: dist
	@(cd i18n; \
	for i in *.po; do \
		msgfmt "$$i" -o ../dist/locale/"$${i%.po}".mo; \
	done)

pot:
	@xgettext -k_ -c -d $(PROGNM) --no-wrap -o i18n/$(PROGNM).pot ./src/*.{c,h}

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
