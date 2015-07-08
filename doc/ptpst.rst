.. Copyright (C) 2015, Dolores Portalatin

ptpst
======

Synopsis
--------

**ptpst**  <*operation*> [*options*]

Description
-----------

:program:`ptpst` is a command-line libcurl C client for pb, a lightweight pastebin.

Operations
----------

-S, --sync
    Create a paste

-R, --remove
    Delete a paste

-U, --update
    Update a paste

Options (global options)
-------

-h, --help
    Print usage information and exit

-P, --provider
    Specify an alternative pb host, default is ptpb.pw

--verbose
    Output verbosely

--version
    List the version and exit

Sync Options (apply to -S)
--------------------------

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
    Render paste from rst to HTML

-v, --vanity=NAME
    Use NAME as a custom Id

-h, --help
    List this help and exit

Remove Options (apply to -R)
----------------------------

-u, --uuid=UUID
    Use UUID as authentication credential

-h, --help
    List this help and exi

Update Options (apply to -U)
----------------------------

-f, --file=FILE
    Use FILE for content of paste

-l, --lexer=LANG
    Lex paste with LANG

-L, --line=LINE
    Highlight LINE

-r, --render
    Render paste from rst to HTML

-u, --uuid=UUID
    Use UUID as authentication credential

-v, --vanity=NAME
    Use NAME as a custom Id

-h, --help
    List this help and exit

Examples
--------

ptpst -Sf <filename>
    pastebin a file to ptpb.pw

ptpst -Ss <url>
    shorten a url with ptpb.pw

ptpst -Ru <UUID>
    remove a paste using the UUID

ptpst -Uu <UUID> -f <filename>
    update a paste with a new file using the UUID

Bugs
----

Report bugs for ptpst to https://github.com/HalosGhost/ptpst/issues

See Also
--------

:manpage:`libcurl(3)`

See the documentation on pb, a lightweight pastebin at https://github.com/ptpb/pb/blob/master/pb/templates/index.rst
