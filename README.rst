pbpst
=====

|coverity|

.. |coverity| image:: https://img.shields.io/coverity/scan/7624.svg
   :alt:              Coverity Scan Build Status
   :target:           https://scan.coverity.com/projects/halosghost-pbpst

``pbpst`` is a small tool to make interacting with ``pb`` instances as simple as possible.
``pb`` is a versatile, free and open source pastebin which has more features, is more robust, and is faster than any other pastebin with which I am familiar.
You can find the reference instance (which tracks `pb's master branch <https://github.com/ptpb/pb>`_) at `ptpb <https://ptpb.pw>`_.
Ideally, ``pbpst`` will support as many features of ``pb`` as possible (hopefully, nearly all) in a simple, unassuming CLI.

Build Requirements and Instructions
-----------------------------------

``pbpst`` only has two external dependencies:

- `libjansson <http://www.digip.org/jansson/>`_
- `libcurl <http://curl.haxx.se/>`_

Note that the versions of these dependencies are not firmly tested, so ``pbpst`` may compile and function just fine on older versions of both libraries.
However, I test only against the latest stable version of both; it is recommended that ``pbpst`` users keep both dependencies up-to-date as well.

In addition to the two external runtime dependencies listed above, ``pbpst`` uses the following three tools to perform a complete build:

- `clang <http://clang.llvm.org/>`_ *
- `python-sphinx <https://pypi.python.org/pypi/Sphinx>`_
- `tup <http://gittup.org/tup/>`_

``clang`` is a special case.
I have recently allowed ``./configure`` to modify both the ``CFLAGS`` and the ``CC`` used in the build (to make packagers' lives simpler), and so you can use ``gcc`` if you would like.
Note that, while I stay pretty strictly in the realm of ISO C, I only test against ``clang``.

The same caveats about versions apply to these make dependencies as apply to the runtime dependencies; tl;dr: keep up-to-date and everything should be okay.

Once you have all the dependencies installed (and you have run ``modprobe fuse`` if you did not already have the fuse filesystem loaded), all you have to do for a complete build of ``pbpst`` is to run the following: ``make``
(Note that the default ``make`` rule just creates the ``dist`` directory and then runs ``tup upd``)

Now, you can simply use ``make DESTDIR=<dest> PREFIX=<pre> install`` to install (or ``make DESTDIR=<dest> PREFIX=<pre> uninstall`` to uninstall).

Pre-built Packages
------------------

If you are a packager and package ``pbpst`` for your distribution, contact me and I will gladly put links to the package here:

* Arch Linux's `[community] repository <https://www.archlinux.org/packages/community/x86_64/pbpst/>`_.
