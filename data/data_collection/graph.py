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
fig, axs = plt.subplots(4, 2, figsize=(12, 12))
fig.suptitle('Plots')

# Plotting Time vs. raw accel and degrees
axs[0, 0].plot(time_points, graphs_data["ACCELRAWTOSCALED"][0], color='blue')
axs[0, 0].set_title('Time vs. Raw Acceleration and Acceleration Degrees')
axs[0, 0].set_xlabel('Time (s)')
axs[0, 0].set_ylabel('Raw Acceleration', color='blue')
axs[0, 0].tick_params(axis='y', labelcolor='blue')

# Creating twin axis for accel degrees
axs2_00 = axs[0, 0].twinx()
axs2_00.plot(time_points, graphs_data["ACCELRAWTOSCALED"][1], color='red')
axs2_00.set_ylabel('Accel Degrees', color='red')
axs2_00.tick_params(axis='y', labelcolor='red')

axs[0, 0].legend(labels=['Raw Acceleration'], loc='lower right')
axs2_00.legend(labels=['Accel Degrees'], loc='upper right')

# Plotting Time vs. Accel degrees and delay length
axs[0, 1].plot(time_points, graphs_data["ACCELSCALEDTODELAYLENGTH"][0], color='blue')
axs[0, 1].set_title('Time vs. Accel Degrees and Delay Length')
axs[0, 1].set_xlabel('Time (s)')
axs[0, 1].set_ylabel('Acceleration Degrees', color='blue')
axs[0, 1].tick_params(axis='y', labelcolor='blue')

# Creating twin axis for delay length
axs2_01 = axs[0, 1].twinx()
axs2_01.plot(time_points, graphs_data["ACCELSCALEDTODELAYLENGTH"][1], color='red')
axs2_01.set_ylabel('Delay Length', color='red')
axs2_01.tick_params(axis='y', labelcolor='red')

# Adding legends for both lines
axs[0, 1].legend(labels=['Raw Accel'], loc='lower right')
axs2_01.legend(labels=['Delay Length'], loc='upper right')


# Plotting Time vs. accel degrees and possible Delay
axs[1, 0].plot(time_points, graphs_data["ACCELSCALEDTOPOSSIBLEDELAY"][0], color='blue')
axs[1, 0].set_title('Time vs. Accel Degrees and Possible Delay')
axs[1, 0].set_xlabel('Time (s)')
axs[1, 0].set_ylabel('Accel Degrees', color='blue')
axs[1, 0].tick_params(axis='y', labelcolor='blue')

# Creating twin axis for possible Delay
axs2_10 = axs[1, 0].twinx()
axs2_10.plot(time_points, graphs_data["ACCELSCALEDTOPOSSIBLEDELAY"][1], color='red')
axs2_10.set_ylabel('Possible Delay Length', color='red')
axs2_10.tick_params(axis='y', labelcolor='red')

axs[1, 0].legend(labels=['Accel Degrees'], loc='lower right')
axs2_10.legend(labels=['Possible Delay Length'], loc='upper right')

# Plotting Time vs. potentiometer and tremolo depth
axs[1, 1].plot(time_points, graphs_data["POTENTSCALEDTODEPTH"][0], color='blue')
axs[1, 1].set_title('Time vs. Raw Potentiometer and Tremolo Depth')
axs[1, 1].set_xlabel('Time (s)')
axs[1, 1].set_ylabel('Potentiometer', color='blue')
axs[1, 1].tick_params(axis='y', labelcolor='blue')

# Creating twin axis for tremolo depth
axs2_11 = axs[1, 1].twinx()
axs2_11.plot(time_points, graphs_data["POTENTSCALEDTODEPTH"][1], color='red')
axs2_11.set_ylabel('Tremolo Depth', color='red')
axs2_11.tick_params(axis='y', labelcolor='red')

axs[1, 1].legend(labels=['Raw Potent'], loc='lower right')
axs2_11.legend(labels=['Tremolo Depth'], loc='upper right')

# Plotting Time vs. accel degrees and tremolo rate
axs[2, 0].plot(time_points, graphs_data["ACCELRAWTOSCALED"][0], color='blue')
axs[2, 0].set_title('Time vs. Accelerometer Degrees and Tremolo Rate')
axs[2, 0].set_xlabel('Time (s)')
axs[2, 0].set_ylabel('Accel Degrees', color='blue')
axs[2, 0].tick_params(axis='y', labelcolor='blue')

# Creating twin axis for tremolo rate
axs2_20 = axs[2, 0].twinx()
axs2_20.plot(time_points, graphs_data["ACCELRAWTOSCALED"][1], color='red')
axs2_20.set_ylabel('Tremolo Rate', color='red')
axs2_20.tick_params(axis='y', labelcolor='red')

axs[2, 0].legend(labels=['Accel Degrees'], loc='lower right')
axs2_20.legend(labels=['Tremolo Rate'], loc='upper right')


# Plotting Time vs. potentiometer and delay repeats
axs[2, 1].plot(time_points, graphs_data["POTENTSCALEDTOREPEATS"][0], color='blue')
axs[2, 1].set_title('Time vs. Potentiometer and Delay Repeats')
axs[2, 1].set_xlabel('Time (s)')
axs[2, 1].set_ylabel('Potentiometer', color='blue')
axs[2, 1].tick_params(axis='y', labelcolor='blue')

# Creating twin axis for delay repeats
axs2_21 = axs[2, 1].twinx()
axs2_21.plot(time_points, graphs_data["POTENTSCALEDTOREPEATS"][1], color='red')
axs2_21.set_ylabel('Delay Repeats', color='red')
axs2_21.tick_params(axis='y', labelcolor='red')

axs[2, 1].legend(labels=['Potent'], loc='lower right')
axs2_21.legend(labels=['Delay Repeats'], loc='upper right')

axs[3, 0].plot(time_points, graphs_data["POTENTRAWTOSCALED"][0], color='blue')
axs[3, 0].set_title('Time vs. Raw Potentiometer and Scaled Potentiometer')
axs[3, 0].set_xlabel('Time (s)')
axs[3, 0].set_ylabel('Raw Potentiometer', color='blue')
axs[3, 0].tick_params(axis='y', labelcolor='blue')

axs2_30 = axs[3, 0].twinx()
axs2_30.plot(time_points, graphs_data["POTENTRAWTOSCALED"][1], color='red')
axs2_30.set_ylabel('Scaled Potentiometer', color='red')
axs2_30.tick_params(axis='y', labelcolor='red')

axs[3, 0].legend(labels=['Raw Potentiometer'], loc='lower right')
axs2_30.legend(labels=['Scaled Potentiometer'], loc='upper right')

axs[3, 1].plot(time_points, graphs_data["EFFECT"], marker='o', linestyle='-')

# Adding labels and title
axs[3, 1].set_xlabel('Time')
axs[3, 1].set_ylabel('Effect')
axs[3, 1].set_title('Time vs. Effect')
axs2_30.legend(labels=['Effect'], loc='upper right')


plt.tight_layout()
plt.show()