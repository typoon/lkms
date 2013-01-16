Hook
====

This is a module used to hijack the sys_call_table for the Linux Kernel 2.6 series and 3.x series.

If it doesn't work for you, you might want to check your System.map file for an address that is before the sys_call_table and an address after it to which you may have access from kernel space and change the for loop to it:

```C
    for (ptr = (unsigned long)sys_close;
         ptr < (unsigned long)&loops_per_jiffy;
         ptr += sizeof(void *)) {
```
