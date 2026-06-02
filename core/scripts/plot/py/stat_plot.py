import matplotlib.pyplot as plt
import numpy as np
import scipy.stats as stats

def show_stats_table(y, threshold_min=None, threshold_max=None):
    try:
        y = np.asarray(y, dtype=np.float64)
        if y.size == 0:
            print("Error: No data available to compute statistics.")
            return False
        if np.isnan(y).all():
            print("Error: All values are NaN.")
            return False

        fig, ax = plt.subplots(figsize=(11, 5))
        ax.axis("off")

        q1 = np.percentile(y, 25)
        q3 = np.percentile(y, 75)
        iqr = q3 - q1
        median = np.median(y)

        mode_result = stats.mode(y, nan_policy='omit', keepdims=True)
        mode_value = mode_result.mode[0]

        std_dev = y.std()
        ci_val = 0.90
        variance = y.var()

        # Compute confidence interval (95%)
        n = len(y)
        if n > 1:
            stderr = std_dev / np.sqrt(n)
            ci_low, ci_high = stats.t.interval(ci_val, df=n-1, loc=y.mean(), scale=stderr)
        else:
            ci_low, ci_high = y.mean(), y.mean()

        stats_data = [
            ["Count",               f"{n}",                           "Total number of values"],
            ["Min",                 f"{y.min():.3f}",                 "Smallest value in the list"],
            ["Max",                 f"{y.max():.3f}",                 "Biggest value in the list"],
            ["Mean",                f"{y.mean():.3f}",                "Average of all values"],
            ["Median",              f"{median:.3f}",                  "Middle value (half above, half below)"],
            ["Mode", f"{mode_value:.3f}", "Most frequently occurring value in the dataset"],
            ["Range",               f"{y.max() - y.min():.3f}",       "Difference between max and min"],
            ["Std Dev",             f"{std_dev*100:.3f}",             "Spread (sensitive to outliers)"],
            ["Variance",            f"{variance*10000:.3f}",          "Spread (square of std dev)"],
            [f"{ci_val * 100}% CI", f"[{ci_low:.3f}, {ci_high:.3f}]", "Confidence interval for the mean"],
            ["Q1 (25%)",            f"{q1:.3f}",                      "25% of values are below this point"],
            ["Q3 (75%)",            f"{q3:.3f}",                      "75% of values are below this point"],
            ["IQR",                 f"{iqr:.3f}",                     "Middle 50% spread (Q3 − Q1)"],
        ]

        table = ax.table(
            cellText=stats_data,
            colLabels=["What", "Value", "Description"],
            cellLoc="left",
            loc="center"
        )

        # Scale the table
        table.scale(1, 1.1)  # Width scale: 1 (no change), Height scale: 2x

        # Set font size for all cells
        for key, cell in table.get_celld().items():
            cell.set_fontsize(20)  # Try 18–22 for high-DPI displays
            cell.set_height(0.1)  # Slightly taller rows

        # plt.subplots_adjust(left=0.1, right=0.9, top=0.9, bottom=0.1)
        fig.tight_layout()  # REMOVE or comment this out
        plt.show()

        return True

    except Exception as e:
        print(f"Error generating statistics table: {e}")
        return False
