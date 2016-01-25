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

- `libjansson 2.7 <http://www.digip.org/jansson/>`_
- `libcurl 7.46.0 <http://curl.haxx.se/>`_

Note that the versions of these dependencies are not firmly tested, so ``pbpst`` may compile and function just fine on older versions of both libraries.
However, I test only against the latest stable version of both; it is recommended that ``pbpst`` users keep both dependencies up-to-date as well.

In addition to the two external runtime dependencies listed above, ``pbpst`` uses the following three tools to perform a complete build:

- `clang 3.7 <http://clang.llvm.org/>`_
- `python-sphinx 1.3.3 <https://pypi.python.org/pypi/Sphinx>`_
- `tup 0.7.3 <http://gittup.org/tup/>`_

The same caveats about versions apply to these make dependencies as apply to the runtime dependencies; tl;dr: keep up-to-date and everything should be okay.

Once you have all the dependencies installed (and you have run ``modprobe fuse`` if you did not already have the fuse filesystem loaded), all you have to do for a complete build of ``pbpst`` is to run the following: ``make``
(Note that the default ``make`` rule just creates the ``dist`` directory and then runs ``tup upd``)

Now, you can simply use ``make DESTDIR=<dest> PREFIX=<pre> install`` to install (or ``make DESTDIR=<dest> PREFIX=<pre> uninstall`` to uninstall).

If you are an Arch Linux user, all the heavy lifting has been done for you, and you can build/install `pbpst-git <https://aur.archlinux.org/packages/pbpst-git/>`_ using ``makepkg`` or your favorite AUR helper if you are so inclined.
