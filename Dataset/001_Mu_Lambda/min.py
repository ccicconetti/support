#!/usr/bin/env python3
"""
Only print rows with given minimum value
"""

import sys
import argparse

parser = argparse.ArgumentParser(
    "Only print rows with given minimum value",
    formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("--delimiter", type=str, default=None,
                    help="Use the specified delimiter")
parser.add_argument("--col", type=int, default=1,
                    help="Which column to use 1-based indexing.")
parser.add_argument("--min", type=float, default=0,
                    help="Minimum value.")
args = parser.parse_args()

data = []
for line in sys.stdin:
    tokens = line.split(args.delimiter)
    if len(tokens) < (args.col - 1):
        raise RuntimeError(f"invalid line: {line}")
    value = float(tokens[args.col - 1])
    if value >= args.min:
        print(line.rstrip())
