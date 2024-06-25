import seaborn as sns
import numpy as np
import math
import os
import matplotlib.pyplot as plt

def list_directories(path):
    try:
        with os.scandir(path) as entries:
            directories = [int(entry.name) for entry in entries if entry.is_dir()]
        return directories
    except FileNotFoundError:
        print(f"The directory '{path}' does not exist.")
        return []
    
def list_files(path):
    try:
        with os.scandir(path) as entries:
            directories = [int(entry.name.replace(".txt", "")) for entry in entries if not entry.is_dir()]
        return directories
    except FileNotFoundError:
        print(f"The directory '{path}' does not exist.")
        return []
    
def save_image(file_path, out_path, index):
    input_file = open(file_path, "r")
    input_data = input_file.readlines()

    results = [[]]
    front_counter = 0
    for idx, line in enumerate(input_data):
        if "=" not in line and line.strip() != "":
            split_line = line.strip().split(":")
            objectives = split_line[0].strip().split(" ")
            pair = [float(objectives[0]), -float(objectives[-1])]
            results[front_counter].append(pair)

        if "=" in line:
            front_counter += 1
            results.append([])
    results = results[:-1]

    palette = sns.color_palette("hsv", len(results))

    plt.figure(figsize=(7, 4))

    for i, front in enumerate(results):
        xs, ys = zip(*front)
        plt.scatter(xs, ys, color=palette[i], label=f'Front {i+1}')

    plt.xlabel('Expected Returns')
    plt.ylabel('Risk')
    plt.title('Generation ' + str(index))
    plt.xticks(np.arange(0, 21, step=2))
    plt.yticks(np.arange(0, 42, step=3))
    plt.legend()
    plt.grid(True)
    plt.savefig(out_path + '/' + str(index) +'.png', dpi=100)
    input_file.close()


path = "./output"
directories = list_directories(path)
directories = sorted(directories)
if len(directories) != 0:
    folderIndex = directories[-1]
    try:
        os.makedirs("./output/" + str(folderIndex) + "/" + str(folderIndex) + "_imgs")
    except FileExistsError:
        pass
    files = list_files("./output/" + str(folderIndex))
    files = sorted(files)
    for file in files:
        curr_file = "./output/" + str(folderIndex) + "/" + str(file) + ".txt"
        out_path = "./output/" + str(folderIndex) + "/" + str(folderIndex) + "_imgs"
        save_image(curr_file, out_path, file)
