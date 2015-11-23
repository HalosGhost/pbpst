PREFIX ?= /usr/local
DOCDIR ?= $(DESTDIR)$(PREFIX)/share/man
BINDIR ?= $(DESTDIR)$(PREFIX)/bin
ZSHDIR ?= $(DESTDIR)$(PREFIX)/share/zsh

.PHONY: install uninstall

install:
	@install -Dm755 src/pbpst      $(BINDIR)/pbpst
	@install -Dm644 doc/pbpst.1    $(DOCDIR)/man1/pbpst.1
	@install -Dm644 doc/pbpst_db.5 $(DOCDIR)/man5/pbpst_db.5
	@install -Dm644 cmp/zsh        $(ZSHDIR)/site-functions/_pbpst

uninstall:
	@rm -f $(BINDIR)/pbpst
	@rm -f $(DOCDIR)/man1/pbpst.1
	@rm -f $(DOCDIR)/man5/pbpst_db.5
	@rm -f $(ZSHDIR)/site-functions/_pbpst
