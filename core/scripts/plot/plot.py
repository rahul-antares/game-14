#!/usr/bin/env python3

import sys

from py import check
from py import parser

def main():
    parser_ = parser.build_parser()
    args = parser_.parse_args()

    if args.require_install_check:
        if not check.run_all_checks(args.verbose):
            print("Environment check failed. Use --verbose to see details.")
            sys.exit(1)
        elif args.verbose:
            print("All required packages are installed.")

    from py import plot
    plot.main(args, parser_.print_help)

if __name__ == "__main__":
    main()
