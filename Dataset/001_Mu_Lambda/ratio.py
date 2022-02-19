#!/usr/bin/env python3
"""
Print the ratio between two columns
"""

import sys
import argparse

parser = argparse.ArgumentParser(
    "Print the ratio between two columns",
    formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("--delimiter", type=str, default=None,
                    help="Use the specified delimiter")
parser.add_argument("--num", type=int, default=1,
                    help="Which column to use for the numerator, 1-based indexing.")
parser.add_argument("--den", type=int, default=2,
                    help="Which column to use for the denumerator, 1-based indexing.")
parser.add_argument("--sort", action="store_true", default=False,
                    help="Sort the values.")
args = parser.parse_args()

data = []
for line in sys.stdin:
    tokens = line.split(args.delimiter)
    if len(tokens) < max(args.num - 1, args.den - 1):
        raise RuntimeError(f"invalid line: {line}")
    num = float(tokens[args.num - 1])
    den = float(tokens[args.den - 1])
    if den != 0:
        data.append(num / den)

if args.sort:
    data = sorted(data)

for d in data:
    print(d)
