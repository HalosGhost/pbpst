.. Copyright (C) 2015, Dolores Portalatin

pbpst
=====

Synopsis
--------

**pbpst**  <*operation*> [*options*]

Description
-----------

:program:`pbpst` is a command-line libcurl C client for pb, a lightweight pastebin.

Operations
----------

-S, --sync
    Create a paste

-R, --remove
    Delete a paste

-U, --update
    Update a paste

Global Options
--------------

-h, --help
    Print usage information and exit

-P, --provider
    Specify an alternative pb host, default is https://ptpb.pw

--verbose
    Output verbosely

--version
    List the version and exit

Sync Options
------------

Note: if neither the :option:`s` or :option:`f` options are passed, :program:`pbpst` will paste the contents of :file:`stdin`.

-s, --shorten=URL
    Create a redirect to URL instead of pasting

-f, --file=FILE
    Create a paste from FILE

-l, --lexer=LANG
    Lex paste with LANG

-L, --line=LINE
    Highlight LINE in paste

-p, --private
    Return a less-guessable Id for paste

-r, --render
    Render a ReStructuredText or Markdown paste to HTML

-v, --vanity=NAME
    Use NAME as a custom Id

-#, --progress
    Show a progress bar for the status of the upload

-h, --help
    List this help and exit

Remove Options
--------------

-u, --uuid=UUID
    Use UUID as authentication credential

-h, --help
    List this help and exit

Update Options
--------------

Note: if the :option:`f` option is not passed, :program:`pbpst` will paste the contents of :file:`stdin`.

-f, --file=FILE
    Use FILE for content of paste

-l, --lexer=LANG
    Lex paste with LANG

-L, --line=LINE
    Highlight LINE

-r, --render
    Render a ReStructuredText or Markdown paste to HTML

-u, --uuid=UUID
    Use UUID as authentication credential

-v, --vanity=NAME
    Use NAME as a custom Id

-#, --progress
    Show a progress bar for the status of the upload

-h, --help
    List this help and exit

Examples
--------

pbpst -Sf <filepath>
    paste the file at <filepath>

pbpst -Ss <url>
    create a shortcut URL to <url>

pbpst -Ru <UUID>
    remove a paste using <UUID> for authentication

pbpst -Uu <UUID> -f <filepath>
    update a paste to the file at <filepath> using <UUID> for authentication

Bugs
----

Report bugs for pbpst to https://github.com/HalosGhost/pbpst/issues

See Also
--------

:manpage:`libcurl(3)`

See the documentation on pb, a lightweight pastebin at https://github.com/ptpb/pb/blob/master/pb/templates/index.rst
