import argparse

def build_parser():
    parser = argparse.ArgumentParser(
        prog="plot-log.py",
        description="Plot a CSV with RTP column. Optionally show stats, image output, and binned gap plots.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )

    parser.add_argument(
        "-w", "--show-window",
        action="store_true",
        default=True,
        help="Display the plot in an interactive window."
    )

    parser.add_argument(
        "-V", "--verbose",
        action="store_true",
        default=False,
        help="Enable verbose output"
    )

    parser.add_argument(
        "-v", "--variant",
        type=int,
        default=96,
        help="RTP Variant"
    )

    parser.add_argument(
        "-r", "--require-install-check",
        action="store_true",
        default=True,
        help="Check for required Python packages"
    )

    parser.add_argument(
        "-i", "--image",
        type=str,
        default=None,
        help="Save the plot as a PNG image to the specified file."
    )

    parser.add_argument(
        "-s", "--stats",
        action="store_true",
        default=True,
        help="Print basic statistics for the RTP values."
    )

    parser.add_argument(
        "-f",
        "--file",
        type=str,
        required=True,
        help="Path to the CSV/LOG file containing ONLY 'RTP' column."
    )

    parser.add_argument(
        "-t", "--title",
        type=str,
        default=None,
        help="Custom title for the plot."
    )

    parser.add_argument(
        "-g", "--gap",
        type=float,
        default=0.1,
        help="Y-axis tick interval"
    )

    parser.add_argument(
        "-m", "--ymin",
        type=float,
        default=95.5,
        help="Minimum y-axis value (auto if not set)"
    )

    parser.add_argument(
        "-M", "--ymax",
        type=float,
        default=96.5,
        help="Maximum y-axis value (auto if not set)"
    )

    return parser
