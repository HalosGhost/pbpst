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

-s, --shorten=URL
    Create a redirect to URL.

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

-P, --provider=HOST
    Use HOST as pb provider; default is https://ptpb.pw.

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

--list-formats
    List the pygments formatters supported by the pb provider and exit.

Sync Options
------------

Note: if the :option:`f` option is not passed, :program:`pbpst` will paste the contents of :file:`stdin`.

-f, --file=FILE
    Create a paste from FILE.

-l, --lexer=LANG
    Lex paste with LANG.

-T, --theme=THEME
    Style paste using the pygments theme THEME.

-e, --extension=EXT
    Specify the MIME-type of the file as EXT.

-L, --line=LINE
    Highlight LINE in paste.

-F, --format=FORM
    Format paste using the pygments format FORM.

-p, --private
    Return a less-guessable Id for paste.

-x, --sunset=SECS
    Set the paste to expire automatically in SECS seconds.

-r, --render
    Render a ReStructuredText or Markdown paste to HTML.

-t, --term
    Handle Asciinema videos.

-v, --vanity=NAME
    Use NAME as a custom Id.

-#, --progress
    Show a progress bar for the status of the upload.

-m, --message=STR
    Store STR as a message with the paste in the local database (helpful for reminding you what the contents of the paste are).

Remove Options
--------------

-u, --uuid=UUID
    Delete (local and upstream) paste identified by UUID.

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

-T, --theme=THEME
    Style paste using the pygments theme THEME.

-e, --extension=EXT
    Specify the MIME-type of the file as EXT.

-L, --line=LINE
    Highlight LINE.

-F, --format=FORM
    Format paste using the pygments format FORM.

-r, --render
    Render a ReStructuredText or Markdown paste to HTML.

-t, --term
    Handle Asciinema videos.

-x, --sunset=SECS
    Set the paste to expire automatically in SECS seconds.

-u, --uuid=UUID
    Use UUID as authentication credential.

-#, --progress
    Show a progress bar for the status of the upload.

-m, --message=STR
    Store STR as a message with the paste in the local database (helpful for reminding you what the contents of the paste are).

Database Options
----------------

-i, --init
    Initialize the database with a default provider and a pastes object, and exit.

-H, --providers
    List all providers currently in the database (one per line).

-q, --query=STR
    Search each paste in the pastes object for text matching STR. Matches will be printed to :file:`stdout` with the form ``<uuid>\t<provider>/<id>\t<msg>\t<sunset>`` where ``<id>`` is either the long id or a vanity label if one exists.

-d, --delete=UUID
    Remove (locally) the paste from the pastes object specified by UUID. Note: This option will respect the default provider and :option:`-P`.

-y, --prune
    Remove all expired paste entries from the local database.
    Equivalent to running ``-Dd`` for each paste with a sunset time older than time at run-time.

Examples
--------

pbpst -Sf <filepath>
    paste the file at <filepath>

pbpst -s <url>
    create a shortcut URL to <url>

pbpst -Ru <UUID>
    remove a paste using <UUID> for authentication

pbpst -Uu <UUID> -f <filepath>
    update a paste to the file at <filepath> using <UUID> for authentication

for i in "$(pbpst -DH)"; do pbpst -Dq '<search>' -P "$i"; done
    search for a paste matching <search> regardless of provider

gpg -o - -c <filepath> | pbpst -S
    encrypt file at <filepath> with GPG symmetric cipher and paste it which can be retrieved and decrypted using ``curl <pasteurl> | gpg -d``

Errors
------

:program:`pbpst` does everything within its power to make sure that user data is preserved wherever possible.
Much of its design follows this principle (for example, this is why there is a swap database).
This means that if it ever sees something go wrong during execution, it tries to fail out in a way that is most likely to preserve user data.

As a result, if something goes wrong and :program:`pbpst` determines it needs to fail out, it may decide to not remove the swap db in case it contains data relevant to the user that needs to be manually merged into the main database.
However, sometimes, :program:`pbpst` may fail out and choose to not clean up the swap db, but the swap db will be empty.
This case triggers a contingency where :program:`pbpst` will detect the empty swap db and clean it up anyway.

Bugs
----

Report bugs for pbpst to https://github.com/HalosGhost/pbpst/issues

Files
-----

$XDG_CONFIG_HOME/pbpst/db.json
$HOME/.config/pbpst/db.json

    One of the two files above contains the lone config option for :program:`pbpst` along with the database of pastes.
    If $XDG_CONFIG_HOME is not defined, the second path will be used as a fallback.

$XDG_CONFIG_HOME/pbpst/.db.json.swp
$HOME/.config/pbpst/.db.json.swp

    One of the two files above contains a copy of the paste database which is used while :program:`pbpst` is running (it is used to try to minimize the possibility of data-loss).
    It will be located in the same directory as the live database.

See Also
--------

:manpage:`libcurl(3)`, :manpage:`pbpst_db(5)`

See the documentation on pb, a lightweight pastebin at https://github.com/ptpb/pb/blob/master/pb/templates/index.rst
