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
    Create a paste.

-R, --remove
    Delete a paste.

-U, --update
    Update a paste.

-D, --database
    Interact with the local database.

Global Options
--------------

The following options are available for any operation.

-h, --help
    Print usage information and exit.

-P, --provider
    Specify an alternative pb host, default is https://ptpb.pw.

-b, --dbpath=PATH
    Specify a file to use as the local database.

--verbose
    Passing once will output the full response from pb in valid yaml (including the URL we create with modifications specified by the user).
    Passing twice will signal :program:`libcurl` to output verbosely as well.

--version
    List the version and exit.

--list-lexers
    List the pygments lexers supported by the pb provider and exit.

--list-themes
    List the pygments themes supported by the pb provider and exit.

Sync Options
------------

Note: if neither the :option:`s` or :option:`f` options are passed, :program:`pbpst` will paste the contents of :file:`stdin`.

-s, --shorten=URL
    Create a redirect to URL instead of pasting.

-f, --file=FILE
    Create a paste from FILE.

-l, --lexer=LANG
    Lex paste with LANG.

-t, --theme=THEME
    Style paste using the pygments theme THEME.

-e, --extension=EXT
    Specify the MIME-type of the file as EXT.

-L, --line=LINE
    Highlight LINE in paste.

-p, --private
    Return a less-guessable Id for paste.

-x, --sunset=SECS
    Set the paste to expire automatically in SECS seconds.

-r, --render
    Render a ReStructuredText or Markdown paste to HTML.

-v, --vanity=NAME
    Use NAME as a custom Id.

-#, --progress
    Show a progress bar for the status of the upload.

-m, --message=STR
    Store STR as a message with the paste in the local database (helpful for reminding you what the contents of the paste are).

Remove Options
--------------

-u, --uuid=UUID
    Use UUID as authentication credential.

-y, --prune
    Delete (local and upstream) all expired pastes.
    Equivalent to running ``-Ru`` for each paste with a sunset time older than time at run-time.

Update Options
--------------

Note: if the :option:`f` option is not passed, :program:`pbpst` will paste the contents of :file:`stdin`.

-f, --file=FILE
    Use FILE for content of paste.

-l, --lexer=LANG
    Lex paste with LANG.

-t, --theme=THEME
    Style paste using the pygments theme THEME.

-e, --extension=EXT
    Specify the MIME-type of the file as EXT.

-L, --line=LINE
    Highlight LINE.

-r, --render
    Render a ReStructuredText or Markdown paste to HTML.

-x, --sunset=SECS
    Set the paste to expire automatically in SECS seconds.

-u, --uuid=UUID
    Use UUID as authentication credential.

-v, --vanity=NAME
    Use NAME as a custom Id.

-#, --progress
    Show a progress bar for the status of the upload.

-m, --message=STR
    Store STR as a message with the paste in the local database (helpful for reminding you what the contents of the paste are).

Database Options
----------------

-i, --init
    Initialize the database with a default provider and a pastes object, and exit.

-q, --query=STR
    Search each paste in the pastes object for text matching STR. Matches will be printed to :file:`stdout` with the form ``<uuid>\t<provider>/<id>\t<msg>\t<sunset>`` where ``<id>`` is either the long id or a vanity label if one exists.

-d, --delete=UUID
    Remove the paste from the pastes object specified by UUID. Note: This option will respect the default provider and :option:`-P`.

-y, --prune
    Remove all expired paste entries from the local database.
    Equivalent to running ``-Dd`` for each paste with a sunset time older than time at run-time.

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
