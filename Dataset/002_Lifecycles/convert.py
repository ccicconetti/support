#!/usr/bin/env python3

import sys
import math

data = []
sys.argv.pop(0)
for (region, arg) in zip(range(len(sys.argv)), sys.argv):
    with open(arg, "r") as infile:
        for line in infile:
            tokens = line.rstrip().split(",")
            assert len(tokens) == 8
            (begin, end, traffic, events, session) = (
                float(tokens[2]),
                float(tokens[3]),
                float(tokens[4]) + float(tokens[5]),
                float(tokens[6]),
                float(tokens[7]),
            )

            if end < begin or session <= 0 or traffic <= 0:
                print(f"skipping line: {line.rstrip()}", file=sys.stderr)
                continue

            duration = end - begin

            # from bytes to bits
            traffic *= 8

            # from ms to s
            duration *= 1e-3
            session *= 1e-3

            # from counters to rates
            traffic /= session
            events /= session

            #
            # compute output
            #

            # occurrence factor
            occurrence = int(duration)

            # processing rate
            processing = min(1, events)

            # key rate
            key = int(math.ceil(math.log10(traffic)))

            data.append((region, occurrence, processing, key))

for d in data:
    print(f"{d[0]},{d[1]},{d[2]},{d[3]}")
