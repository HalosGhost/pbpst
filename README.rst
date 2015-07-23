pbpst
=====

``pbpst`` is a small tool to make interacting with ``pb`` instances as simple as possible.
``pb`` is a versatile, free and open source pastebin which has more features, is more robust and is faster than any other pastebin with which I am familiar.
You can find the reference instance (which tracks `pb's master branch <https://github.com/ptpb/pb>`_) at `ptpb <https://ptpb.pw>`_.
Ideally, ``pbpst`` will support as many features of ``pb`` as possible (hopefully, nearly all) in a simple, unassuming CLI.

To-do
-----

- [X] Pasting
- [X] Piping through ``stdin``
- [X] URL Shortening
- [X] Updating pastes
- [X] Deleting pastes
- [X] Vanity pastes
- [X] Private pastes
- [X] Alternative Providers
- [X] Add generated man pages to the docs section
- [ ] Rendering handler (now supports Rst and Mdown)
- [ ] Lexers

  - [ ] Option to list all available lexers

- [ ] Line highlighting
- [ ] Arbitrary Pygments themes
- [ ] Database of owned pastes (probably in JSON with libjansson)

  - [ ] Ability to manually remove (, add?) and query pastes in the database
  - [ ] Should double as the local config file (e.g., should specify the default provider to use)

- [ ] zsh completion of UUIDs in db to make removal super simple
- [ ] Add test suite
