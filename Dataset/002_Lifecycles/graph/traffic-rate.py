#!/usr/bin/env python3

import sys

for line in sys.stdin:
    tokens = line.rstrip().split(",")
    assert len(tokens) == 7
    dur = ( float(tokens[3]) - float(tokens[2]) ) * 1e-3
    assert dur > 0.0
    data = 8.0 * ( float(tokens[4]) + float(tokens[5])  )
    print(f'{data/dur}')
