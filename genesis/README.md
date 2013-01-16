Genesis
=======

This module will let you create a char device on the system. When the string "genesis" is written into this device it will make the process that is writting it become uid and euid 0 (root).

Usage
=====

1 Compile the module using the Makefile
2 cat /proc/devices |grep genesis
3 Grab the number that shows sided to it
4 cd /dev
5 mknod gensis c <number_from_step_3> 1
6 chmod o+w /dev/genesis

In order to try it, execute the get_root tool provided together in the test/ directory
