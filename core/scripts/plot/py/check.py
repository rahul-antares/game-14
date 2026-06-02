import sys
import shutil
import importlib.util

REQUIRED_PYTHON = (3, 7)
REQUIRED_PACKAGES = ["pandas", "matplotlib", "numpy", "seaborn", "scipy"]

def check_python_version(verbose=False):
    if sys.version_info < REQUIRED_PYTHON:
        print(f"Python {REQUIRED_PYTHON[0]}.{REQUIRED_PYTHON[1]}+ required. Found: {sys.version.split()[0]}")
        return False

    return True

def check_pip(verbose=False):
    if shutil.which("pip") is None and shutil.which("pip3") is None:
        print("pip is not installed or not found in PATH.")
        return False

    return True

def check_packages(verbose=False):
    missing = []

    for pkg in REQUIRED_PACKAGES:
        if importlib.util.find_spec(pkg) is None:
            missing.append(pkg)

    if missing:
        print("\nMissing required packages:")
        for pkg in missing:
            print(f"  - {pkg}")

        print("\nTo install with pip:")
        print(f"  pip install {' '.join(missing)}")
        print(f"  pip3 install {' '.join(missing)}")

        print("\nTo install on Ubuntu:")
        print(f"  sudo apt install {' '.join(f'python3-{pkg}' for pkg in missing)}")

        print("")
        return False

    if verbose:
        print("All required packages are installed.")

    return True

def run_all_checks(verbose=False):
    print(f"Using Python interpreter: {sys.executable}")
    ok_python = check_python_version(verbose)
    ok_pip = check_pip(verbose)
    ok_pkgs = check_packages(verbose)
    return ok_python and ok_pip and ok_pkgs
