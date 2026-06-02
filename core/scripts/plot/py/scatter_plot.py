import math

def x_tick_interval(iters):
    iter_interval_trivial = max(1, iters / 10)
    n_digits = math.floor(math.log10(iter_interval_trivial))

    print(f"#digits = {n_digits}")

    closest_10_power = math.pow(10, n_digits)
    print(f"closest power(10) = {closest_10_power}")

    mults = [1, 2, 2.5, 5, 10]
    diffs = []

    for m in mults:
        candidate = m * closest_10_power
        diff = abs(candidate - iter_interval_trivial)
        diffs.append((diff, candidate))

    diffs.sort()

    for diff, val in diffs:
        print(f"({diff}, {val})", end=' ')
    print()

    return diffs[0][1]

def plot_rtp(args, y):
    import matplotlib.pyplot as plt
    import numpy as np
    import matplotlib.ticker as ticker

    try:
        y = np.asarray(y, dtype=np.float64)
        if y.size == 0:
            print("Error: No data to plot.")
            return False
        if np.isnan(y).all():
            print("Error: Data contains only NaN values.")
            return False

        x = range(len(y))
        plt.figure(figsize=(12, 6))

        if args.gap:
            plt.gca().yaxis.set_major_locator(ticker.MultipleLocator(args.gap))

        # Define color coding rules
        severity_ranges_96 = [
            ((0, 95.5), 'red', 'Severe Low'),
            ((95.5, 95.8), 'salmon', 'Moderate Low'),
            ((95.8, 95.988), 'orange', 'Somewhat Low'),
            ((95.988, 96.012), 'green', 'Optimal'),
            ((96.012, 96.2), 'orange', 'Somewhat High'),
            ((96.2, 96.5), 'salmon', 'Moderate High'),
            ((96.5, 100), 'red', 'Severe High'),
        ]

        severity_ranges_94 = [
            ((0, 93.5), 'red', 'Severe Low'),
            ((93.5, 93.8), 'orange', 'Moderate Low'),
            ((93.8, 93.989), 'orange', 'Somewhat Low'),
            ((93.989, 94.011), 'green', 'Optimal'),
            ((94.011, 94.2), 'orange', 'Somewhat High'),
            ((94.2, 94.5), 'orange', 'Moderate High'),
            ((94.5, 100), 'red', 'Severe High'),
        ]

        severity_ranges_92 = [
            ((0, 91.5), 'red', 'Severe Low'),
            ((91.5, 91.8), 'orange', 'Moderate Low'),
            ((91.8, 91.988), 'orange', 'Somewhat Low'),
            ((91.988, 92.012), 'green', 'Optimal'),
            ((92.012, 92.2), 'orange', 'Somewhat High'),
            ((92.2, 92.5), 'orange', 'Moderate High'),
            ((92.5, 100), 'red', 'Severe High'),
        ]

        severity_ranges = []

        if args.variant == 96:
            severity_ranges = severity_ranges_96
        elif args.variant == 94:
            severity_ranges = severity_ranges_94
        elif args.variant == 92:
            severity_ranges = severity_ranges_92

        # Function to determine color based on value
        def get_color(value):
            for (low, high), color, label in severity_ranges:
                if (low is None or value >= low) and (high is None or value <= high):
                    return color
            return 'gray'

        colors = [get_color(val) for val in y]


        import matplotlib.patches as mpatches

        # To store unique entries based on label
        seen_labels = set()
        legend_patches = []

        for (low, high), color, label in severity_ranges:
            if label not in seen_labels:
                range_str = f"{low} – {high}"
                full_label = f"{label} ({range_str})"
                patch = mpatches.Patch(color=color, label=full_label)
                legend_patches.append(patch)
                seen_labels.add(label)


        n_iters = len(y)
        point_size = 1 if n_iters > 200 else 5 if n_iters > 100 else 10
        # x_label = 
        plt.scatter(x, y, c=colors, label="Scatter Plot RTP", s=point_size)

        # X-axis tick interval and label formatting
        xtick_interval = x_tick_interval(n_iters)
        xtick_format = "b" if n_iters >= 1000 else "m"

        plt.gca().xaxis.set_major_locator(ticker.MultipleLocator(xtick_interval))

        if xtick_format == "b":
            plt.xlabel("Iterations (billions)")
            formatter = ticker.FuncFormatter(lambda val, pos: f'{val / 1000:.2f}B')
        else:
            plt.xlabel("Iterations (millions)")
            formatter = ticker.FuncFormatter(lambda val, pos: f'{int(val)}M')
        plt.gca().xaxis.set_major_formatter(formatter)

        plt.ylabel("RTP")

        title = args.title if args.title else getattr(args, "default_title", "")
        title += f" Scatter Plot for Variant({args.variant})"
        plt.title(title)

        ymin = args.ymin if args.ymin is not None else y.min() - 0.1
        ymax = args.ymax if args.ymax is not None else y.max() + 0.1
        plt.ylim(ymin, ymax)

        plt.grid(True)
        plt.tight_layout()
        plt.legend(handles=legend_patches, title="Severity Ranges(RTP)")

        if args.image:
            try:
                plt.savefig(args.image)
                print(f"Saved image to: {args.image}")
            except Exception as e:
                print(f"Error saving image: {e}")
                return False

        if getattr(args, "show_window", True):
            plt.show()

        return True

    except Exception as e:
        print(f"Error while generating RTP scatter plot: {e}")
        return False
