import matplotlib.pyplot as plt

# Initialize dictionaries to store data
graphs_data = {
    "ACCELRAWTODELAYLENGTH": [[], []],
    "POTENTRAWTODEPTH": [[], []],
    "POTENTRAWTOREPEATS": [[], []],
    "ACCELRAWTODEG": [[], []],
    "ACCELRAWTORATE": [[], []],
    "ACCELRAWTOPOSSIBLEDELAY": [[], []]
}

# Read data from file and parse each line
with open("alldata.txt", "r") as file:
    for line in file:
        if line.startswith("$"):
            key, *values = line.strip().split(',')
            graph_name = key.strip("$")
            try:
                graphs_data[graph_name][0].append(float(values[0]))
                graphs_data[graph_name][1].append(float(values[1]))
            except Exception:
                print("lol")
                break

# # Calculate time points
# time_points = [i/10 for i in range(len(graphs_data["ACCELRAWTODELAYLENGTH"][0]))]

# # Plot all graphs together
# fig, axs = plt.subplots(3, 2, figsize=(12, 12))
# fig.suptitle('Plots')

# axs[0, 0].plot(time_points, graphs_data["ACCELRAWTODELAYLENGTH"][0])
# axs[0, 0].plot(time_points, graphs_data["ACCELRAWTODELAYLENGTH"][1])
# axs[0, 0].set_title('Time vs. Accel degrees and delay length')
# axs[0, 0].set_xlabel('Time (s)')
# axs[0, 0].set_ylabel('Acceleration Degrees')
# axs[0, 0].legend(labels=['Raw Accel', 'Delay Length'])

# axs[0, 1].plot(time_points, graphs_data["POTENTRAWTODEPTH"][0])
# axs[0, 1].plot(time_points, graphs_data["POTENTRAWTODEPTH"][1])
# axs[0, 1].set_title('Time vs. potentiometer and tremolo depth')
# axs[0, 1].set_xlabel('Time (s)')
# axs[0, 1].set_ylabel('Potentiometer')
# axs[0, 1].legend(labels=['Raw Potent', 'Tremolo Depth'])

# axs[1, 0].plot(time_points, graphs_data["POTENTRAWTOREPEATS"][0])
# axs[1, 0].plot(time_points, graphs_data["POTENTRAWTOREPEATS"][1])
# axs[1, 0].set_title('Time vs. potentiometer and delay repeats')
# axs[1, 0].set_xlabel('Time (s)')
# axs[1, 0].set_ylabel('Tremolo Depth')
# axs[1, 0].legend(labels=['Raw Potent', 'Delay Repeats'])

# axs[1, 1].plot(time_points, graphs_data["ACCELRAWTODEG"][0])
# axs[1, 1].plot(time_points, graphs_data["ACCELRAWTODEG"][1])
# axs[1, 1].set_title('Time vs. raw accel and degrees')
# axs[1, 1].set_xlabel('Time (s)')
# axs[1, 1].set_ylabel('Potentiometer')
# axs[1, 1].legend(labels=['Raw Accel', 'Accel Degrees'])

# axs[2, 0].plot(time_points, graphs_data["ACCELRAWTORATE"][0])
# axs[2, 0].plot(time_points, graphs_data["ACCELRAWTORATE"][1])
# axs[2, 0].set_title('Time vs. accel degrees and tremolo rate')
# axs[2, 0].set_xlabel('Time (s)')
# axs[2, 0].set_ylabel('Delay Repeats')
# axs[2, 0].legend(labels=['Accel Degrees', 'Tremolo Rate'])

# axs[2, 1].plot(time_points, graphs_data["ACCELRAWTOPOSSIBLEDELAY"][0])
# axs[2, 1].plot(time_points, graphs_data["ACCELRAWTOPOSSIBLEDELAY"][1])
# axs[2, 1].set_title('Time vs. accel degrees and possible Delay')
# axs[2, 1].set_xlabel('Time (s)')
# axs[2, 1].set_ylabel('Delay Repeats')
# axs[2, 1].legend(labels=['Accel Degrees', 'Possible Delay Length'])

# plt.tight_layout()
# plt.show()


# Calculate time points
time_points = [i/10 for i in range(len(graphs_data["ACCELRAWTODELAYLENGTH"][0]))]

# Plot all graphs together
fig, axs = plt.subplots(3, 2, figsize=(12, 12))
fig.suptitle('Plots')

# Plotting Time vs. Accel degrees and delay length
axs[0, 0].plot(time_points, graphs_data["ACCELRAWTODELAYLENGTH"][0], color='blue')
axs[0, 0].set_title('Time vs. Accel degrees and delay length')
axs[0, 0].set_xlabel('Time (s)')
axs[0, 0].set_ylabel('Acceleration Degrees', color='blue')
axs[0, 0].tick_params(axis='y', labelcolor='blue')
axs[0, 0].legend(labels=['Raw Accel'], loc='upper left')

# Creating twin axis for delay length
axs2_00 = axs[0, 0].twinx()
axs2_00.plot(time_points, graphs_data["ACCELRAWTODELAYLENGTH"][1], color='red')
axs2_00.set_ylabel('Delay Length', color='red')
axs2_00.tick_params(axis='y', labelcolor='red')
axs[0, 0].legend(labels=['Raw Accel', 'Delay Length'], loc='upper right')

# Plotting Time vs. potentiometer and tremolo depth
axs[0, 1].plot(time_points, graphs_data["POTENTRAWTODEPTH"][0], color='blue')
axs[0, 1].set_title('Time vs. potentiometer and tremolo depth')
axs[0, 1].set_xlabel('Time (s)')
axs[0, 1].set_ylabel('Potentiometer', color='blue')
axs[0, 1].tick_params(axis='y', labelcolor='blue')
axs[0, 1].legend(labels=['Raw Potent'], loc='upper left')

# Creating twin axis for tremolo depth
axs2_01 = axs[0, 1].twinx()
axs2_01.plot(time_points, graphs_data["POTENTRAWTODEPTH"][1], color='red')
axs2_01.set_ylabel('Tremolo Depth', color='red')
axs2_01.tick_params(axis='y', labelcolor='red')
axs[0, 1].legend(labels=['Raw Potent', 'Tremolo Depth'], loc='upper right')

# Plotting Time vs. potentiometer and delay repeats
axs[1, 0].plot(time_points, graphs_data["POTENTRAWTOREPEATS"][0], color='blue')
axs[1, 0].set_title('Time vs. potentiometer and delay repeats')
axs[1, 0].set_xlabel('Time (s)')
axs[1, 0].set_ylabel('Potentiometer', color='blue')
axs[1, 0].tick_params(axis='y', labelcolor='blue')
axs[1, 0].legend(labels=['Raw Potent'], loc='upper left')

# Creating twin axis for delay repeats
axs2_10 = axs[1, 0].twinx()
axs2_10.plot(time_points, graphs_data["POTENTRAWTOREPEATS"][1], color='red')
axs2_10.set_ylabel('Delay Repeats', color='red')
axs2_10.tick_params(axis='y', labelcolor='red')
axs[1, 0].legend(labels=['Raw Potent', 'Delay Repeats'], loc='upper right')

# Plotting Time vs. raw accel and degrees
axs[1, 1].plot(time_points, graphs_data["ACCELRAWTODEG"][0], color='blue')
axs[1, 1].set_title('Time vs. raw accel and degrees')
axs[1, 1].set_xlabel('Time (s)')
axs[1, 1].set_ylabel('Acceleration Degrees', color='blue')
axs[1, 1].tick_params(axis='y', labelcolor='blue')
axs[1, 1].legend(labels=['Raw Accel'], loc='upper left')

# Creating twin axis for accel degrees
axs2_11 = axs[1, 1].twinx()
axs2_11.plot(time_points, graphs_data["ACCELRAWTODEG"][1], color='red')
axs2_11.set_ylabel('Accel Degrees', color='red')
axs2_11.tick_params(axis='y', labelcolor='red')
axs[1, 1].legend(labels=['Raw Accel', 'Accel Degrees'], loc='upper right')

# Plotting Time vs. accel degrees and tremolo rate
axs[2, 0].plot(time_points, graphs_data["ACCELRAWTORATE"][0], color='blue')
axs[2, 0].set_title('Time vs. accel degrees and tremolo rate')
axs[2, 0].set_xlabel('Time (s)')
axs[2, 0].set_ylabel('Accel Degrees', color='blue')
axs[2, 0].tick_params(axis='y', labelcolor='blue')
axs[2, 0].legend(labels=['Accel Degrees'], loc='upper left')

# Creating twin axis for tremolo rate
axs2_20 = axs[2, 0].twinx()
axs2_20.plot(time_points, graphs_data["ACCELRAWTORATE"][1], color='red')
axs2_20.set_ylabel('Tremolo Rate', color='red')
axs2_20.tick_params(axis='y', labelcolor='red')
axs[2, 0].legend(labels=['Accel Degrees', 'Tremolo Rate'], loc='upper right')

# Plotting Time vs. accel degrees and possible Delay
axs[2, 1].plot(time_points, graphs_data["ACCELRAWTOPOSSIBLEDELAY"][0], color='blue')
axs[2, 1].set_title('Time vs. accel degrees and possible Delay')
axs[2, 1].set_xlabel('Time (s)')
axs[2, 1].set_ylabel('Accel Degrees', color='blue')
axs[2, 1].tick_params(axis='y', labelcolor='blue')
axs[2, 1].legend(labels=['Accel Degrees'], loc='upper left')

# Creating twin axis for possible Delay
axs2_21 = axs[2, 1].twinx()
axs2_21.plot(time_points, graphs_data["ACCELRAWTOPOSSIBLEDELAY"][1], color='red')
axs2_21.set_ylabel('Possible Delay Length', color='red')
axs2_21.tick_params(axis='y', labelcolor='red')
axs[2, 1].legend(labels=['Accel Degrees', 'Possible Delay Length'], loc='upper right')

# Repeat the same pattern for the rest of the subplots...

plt.tight_layout()
plt.show()
