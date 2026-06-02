import os

def main(args, usage):
    import pandas as pd

    try:
        from . import to_csv

        try:
            if args.file.endswith(".log"):
                df = to_csv.get_rtp_dataframe(args.file)
                df["RTP"] = pd.to_numeric(df["RTP"], errors="coerce")
            else:
                df = pd.read_csv(args.file)
        except Exception as e:
            print(f"Error reading file: {e}\n\n")
            usage()
            return

        if "RTP" not in df.columns:
            print("Error: Input must contain a column named 'RTP'")
            return
    except Exception as e:
        print(f"Error reading file: {e}\n\n")
        usage()
        return

    y = df["RTP"].dropna()
    args.default_title = os.path.splitext(os.path.basename(args.file))[0]

    from . import scatter_plot
    if not scatter_plot.plot_rtp(args, y):
        print("Failed to create RTP scatter plot.")

    # from . import stat_plot
    # if not stat_plot.show_stats_table(y):
    #     print("Failed to generate statistics table.")

    # from py import density_plot
    # success = density_plot.show_density_plot(y, bandwidth=0.5, show_hist=True, title="Density Example")
    # if not success:
    #     print("Density plot generation failed.")
