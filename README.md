drbdstat
========

Display DRBD status continuously with adjustable rate of update

Compile with: gcc -o drbdstat drbdstat.c

Although the function of this program can largely be duplicated simply by running, e.g.:

  `watch cat /proc/drbdstat`,
  
... we add the ability to adjust the update speed of the display.
