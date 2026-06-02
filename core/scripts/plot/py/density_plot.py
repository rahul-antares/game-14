import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

def show_density_plot(y, bandwidth=None, show_hist=True, title="Density Plot"):
    """
    Show a density plot (KDE) for the given data.
    
    Parameters:
        y (array-like): The numeric data.
        bandwidth (float, optional): Kernel bandwidth for smoothing. If None, Seaborn chooses.
        show_hist (bool): Whether to include a histogram behind the KDE.
        title (str): Title of the plot.
    
    Returns:
        success (bool): True if the plot succeeded, False if there was an error.
    """
    try:
        # Ensure y is a numeric array
        y = np.asarray(y, dtype=np.float64)
        if y.size == 0:
            print("Error: No data to plot.")
            return False
        if np.isnan(y).all():
            print("Error: Data contains only NaN values.")
            return False

        fig, ax = plt.subplots(figsize=(10, 4))
        sns.set(style="whitegrid")

        # KDE plot
        sns.kdeplot(y, bw_adjust=bandwidth, fill=True, label="KDE", ax=ax)

        # Optional histogram
        if show_hist:
            sns.histplot(y, bins=30, kde=False, color="lightgray", label="Histogram", ax=ax)

        ax.set_title(title)
        ax.set_xlabel("Value")
        ax.set_ylabel("Density")
        ax.legend()
        plt.tight_layout()
        plt.show()

        return True

    except Exception as e:
        print(f"Error while generating density plot: {e}")
        return False
