import os

def list_directories(path):
    try:
        with os.scandir(path) as entries:
            directories = [int(entry.name) for entry in entries if entry.is_dir()]
        return directories
    except FileNotFoundError:
        print(f"The directory '{path}' does not exist.")
        return []

path = "./output"
directories = list_directories(path)
directories = sorted(directories)
if len(directories) == 0:
    print(1)
else:
    print(directories[-1] + 1)
