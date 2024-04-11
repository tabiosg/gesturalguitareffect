import matplotlib.pyplot as plt

# Initialize dictionaries to store data
graphs_data = {
    "ACCELSCALEDTODELAYLENGTH": [[], []],
    "POTENTSCALEDTODEPTH": [[], []],
    "POTENTSCALEDTOREPEATS": [[], []],
    "ACCELRAWTOSCALED": [[], []],
    "ACCELSCALEDTORATE": [[], []],
    "POTENTRAWTOSCALED": [[], []],
    "EFFECT": [],
    "ACCELSCALEDTOPOSSIBLEDELAY": [[], []]
}

# Read data from file and parse each line
with open("alldata.txt", "r") as file:
    for line in file:
        if line.startswith("$"):
            key, *values = line.strip().split(',')
            graph_name = key.strip("$")
            if graph_name == "EFFECT":
                graphs_data["EFFECT"].append(values[0])
            else:
                graphs_data[graph_name][0].append(float(values[0]))
                graphs_data[graph_name][1].append(float(values[1]))

# Calculate time points
time_points = [i/10 for i in range(len(graphs_data["ACCELRAWTOSCALED"][0]))]

# Plot all graphs together
fig, axs = plt.subplots(1, 1, figsize=(12, 12))

# Plotting Time vs. raw accel and degrees
axs.plot(time_points, graphs_data["ACCELRAWTOSCALED"][0], color='blue')
axs.set_title('Time vs. Raw Acceleration and Acceleration Degrees')
axs.set_xlabel('Time (s)')
axs.set_ylabel('Raw Acceleration', color='blue')
axs.tick_params(axis='y', labelcolor='blue')

# Creating twin axis for accel degrees
axs2_00 = axs.twinx()
axs2_00.plot(time_points, graphs_data["ACCELRAWTOSCALED"][1], color='red')
axs2_00.set_ylabel('Accel Degrees', color='red')
axs2_00.tick_params(axis='y', labelcolor='red')

axs.legend(labels=['Raw Acceleration'], loc='lower right')
axs2_00.legend(labels=['Accel Degrees'], loc='upper right')

plt.tight_layout()
plt.show()
