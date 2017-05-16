#!/usr/bin/env python

import os

def conv_unit(number):
    units = {"K": "1024", "M": "1024 * 1024"}
    for u in units.keys():
        number = number.replace(u, " * " + units[u])
    return number


# main procedure

print "#ifndef _SYSCONFIG_H"
print "#define _SYSCONFIG_H\n"

print "#define SZ_WORD\tsizeof(unsigned long long)"

cache_info_path = "/sys/devices/system/cpu/cpu0/cache/index"

sz_cl_fpath = cache_info_path + "0/coherency_line_size"
with open(sz_cl_fpath, 'r') as f:
    sz_cl = f.read().strip()
sz_cl = "(" + conv_unit(sz_cl) + ")"
print "#define SZ_CACHELINE\t%s" % sz_cl

for lv in [1, 2, 3]:
    dirname = lv
    if dirname == 1:
        dirname = 0

    fpath = "/sys/devices/system/cpu/cpu0/cache/index%s/size" % dirname
    if not os.path.exists(fpath):
        continue

    with open(fpath, 'r') as f:
        csize = f.read().strip()

    csize = "(" + conv_unit(csize) + ")"
    print "#define SZ_L%d\t%s" % (lv, csize)

print "\n#endif"
