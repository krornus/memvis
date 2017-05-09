#!/bin/sh
for p in $(find /root/ind_study/memvis/python/rules -type f -name "*.yar"); do yara -w $p $1;done
