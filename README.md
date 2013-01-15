Stuzi
=====


Installation
------------

    $ git clone git://github.com/vinc/stuzi.git
    $ cd stuzi
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ sudo make install


Usage
-----

    $ stuzi -s
    Fetching 'ftp://ftp.ripe.net/pub/stats/afrinic/delegated-afrinic-latest' ...
    Fetching 'ftp://ftp.ripe.net/pub/stats/apnic/delegated-apnic-latest' ...
    Fetching 'ftp://ftp.ripe.net/pub/stats/arin/delegated-arin-latest' ...
    Fetching 'ftp://ftp.ripe.net/pub/stats/lacnic/delegated-lacnic-latest' ...
    Fetching 'ftp://ftp.ripe.net/pub/stats/ripencc/delegated-ripencc-latest' ...

    $ stuzi 8.8.8.8
    US
