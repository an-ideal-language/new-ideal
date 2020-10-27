# new-ideal

A second implementation of the ideal constraint-based picture description
language.  The relation between this code and the code in the earlier implementation (`ideal`) is unknown.

The original code was copied from
[freaknet.org/martin/tape/stuff/ditroff/newideal](http://freaknet.org/martin/tape/stuff/ditroff/newideal/) .

The files downloaded are:

```
  Makefile    1987-06-30 14:17    1.6K
  RAW_FORMAT  1987-04-29 09:24    1.4K
  README      1987-04-29 09:24    1.6K
  action.c    1987-04-29 09:24    9.1K
  ideal.c     1987-04-29 09:24    4.0K
  ideal.cmd   1987-04-29 09:24    1.2K
  ideal.h     1987-04-29 09:24    8.0K
  ideal.sh    1987-06-30 16:50    1.2K
  idfilt/     2002-03-15 18:59    -
    4014.c       1987-04-29 09:24 2.1K
    Makefile     1987-04-29 09:24 1.3K
    har.c        1987-04-29 09:24 4.0K
    idfilt.c     1987-04-29 09:24 5.0K
    idfilt.h     1987-04-29 09:24 1.0K
    idsort.c     1987-04-29 09:24 3.9K
    pout.c       1987-04-29 09:24 2.0K
    ver.c        1987-04-29 09:24 4.0K
  idlex.l     1987-04-29 09:24    6.2K
  idyac.y     1987-04-29 09:24    4.9K
  inter.c     1987-04-29 09:24    4.2K
  lib/        1990-07-25 15:32    -
    ARC         1987-04-29 09:24  1.0K
    Arc         1987-04-29 09:24  830
    CIRCLE      1987-04-29 09:24  438
    Makefile    1987-04-29 09:24  547
    a           1987-04-29 09:24  115
    arc         1987-04-29 09:24  115
    arrow       1987-04-29 09:24  199
    bar         1987-04-29 09:24  513
    c           1987-04-29 09:24  193
    circle      1987-04-29 09:24  193
    dash        1987-04-29 09:24  96
    dashed      1987-04-29 09:24  125
    hole        1987-04-29 09:24  191
    rect        1987-04-29 09:24  280
    wavy        1987-04-29 09:24  202
  make.out    1987-04-29 09:24    1.2K
  memut.c     1987-04-29 09:24    12K
  opaque.c    1987-04-29 09:24    7.1K
  opqpoly.c   1987-04-29 09:24    22K
  piece.c     1987-04-29 09:24    1.6K
  simul.c     1987-04-29 09:24    2.3K
  stdas.h     1987-04-29 09:24    471
  util.c      1987-04-29 09:24    6.9k
```

The code as downloaded is tagged original-code.  Some of these files are corruped; in particular `action.c`
is seems to be discontinuous around line n, and it ends with a copy of the man-page source.
