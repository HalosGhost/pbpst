ptpst
=====

Eventually, this repo will play host to the source for a replacement for my command-line client for ``pomf``.
``pomf`` was a decent tool, but was essentially useless in terms of its use as a pastebin and its author has said some… questionable things.
`ptpb <https://ptpb.pw>`_, on the other hand, is the most versatile pastebin I have ever seen while still maintaining all the power of ``pomf`` (with a larger filesize cap too!).

The goal of this utility is to support as many features of ``ptpb`` as possible (hopefully, all) in a simple cli.

To-do
-----

- [X] Pasting
- [X] Piping through ``stdin``
- [X] URL Shortening
- [-] RST rendering handler
- [-] Lexers
- [-] Line highlighting
- [X] Updating pastes
- [X] Deleting pastes
- [X] Vanity pastes
- [X] Private pastes
- [ ] Database of owned pastes

  - [ ] Ability to manually add or remove pastes from the db

- [ ] zsh completion of UUIDs in db to make removal super simple

