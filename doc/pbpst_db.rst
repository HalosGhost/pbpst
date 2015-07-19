.. Copyright (C) 2015, Sam Stuewe

pbpst_db
========

Description
-----------

The local, human-readable (JSON) database and configuration for :program:`pbpst`.

This database automatically tracks what pastes a user makes to pb deployments.
As a result, management of what pastes a user makes should be dramatically simplified.
For example, since it will record the UUID for the user, the need to memorize, copy down or otherwise manually record the authentication information is no longer necessary.

Schema
------

{}
    The top-level of the database contains a solitary object.

default_provider
    A string equal to the URL of the provider that :program:`pbpst` should use by-default

pastes
    An object containing objects (one for each pb provider that has been used).
    The key for one of these objects is the URL of the provider of the pastes it contains.
    It may be quite common for there to only be one object beneath: the provider specified by ``default_provider``.

<url>
    An object containing paste objects.
    The key for one of these paste objects is the UUID associated with that paste.
    This level of abstraction is included mostly so that a user can simply use a single database to interact with multiple pb providers.

<uuid>
    A paste object.

long
    A string equal to the long identifier associated with the paste.
    This is a left-zero-padded (to 21 bytes) base64 encoding of the sha1 of the paste contents.

label
    A string equal to the vanity label associated with the paste (if one was given).
    If the paste does not have a vanity label, this will be set to ``null``.

msg
    An arbitrary string which may be specified by the user upon creation/updating of the associated paste.
    If the user choose not to specify a string for a paste, this field defaults to the basename of the file.

Example
-------

.. code:: json

    {
        "default_provider": "https://ptpb.pw",
        "pastes": {
            "https://ptpb.pw": {
                "fa3423b6-2c29-4758-a707-bf3f972b93c9": {
                    "long": "AFKg4i_J0ssH6sJKdsYCA8WuR542",
                    "label": "random_junk",
                    "msg": "-"
                }
            }
        }
    }

The above snippet is an example database that keeps the default provider and is tracking one paste (which happens to be on that default provider).
The ``msg`` field is "-" because this paste was created by piping through :file:`stdin`.

Bugs
----

Report bugs for pbpst to https://github.com/HalosGhost/pbpst/issues

See Also
--------

:manpage:`pbpst(1)`

See the documentation on pb, a lightweight pastebin at https://github.com/ptpb/pb/blob/master/pb/templates/index.rst
