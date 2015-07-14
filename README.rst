pbpst
=====

Eventually, this repo will play host to the source for a replacement for my command-line client for ``pomf``.
``pomf`` was a decent tool, but was essentially useless in terms of its use as a pastebin and its author has said some… questionable things.
`ptpb <https://ptpb.pw>`_, on the other hand, is the most versatile pastebin I have ever seen while still maintaining all the power of ``pomf`` (with a larger filesize cap too!).

The goal of this utility is to support as many features of ``pb`` as possible (hopefully, all) in a simple cli.

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
- [ ] Rendering handler (now supports Rst and Mdown)
- [ ] Lexers
- [ ] Line highlighting
- [ ] Arbitrary Pygments themes
- [ ] Database of owned pastes (probably in JSON with libjansson)

  - [ ] Ability to manually add or remove pastes from the db
  - [ ] Should double as the local config file (e.g., should specify the default provider to use

- [ ] zsh completion of UUIDs in db to make removal super simple
- [ ] Add test suite
- [X] Add generated man pages to the docs section
