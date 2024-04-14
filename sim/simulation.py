import tkinter as tk
from tkinter import ttk
import pyaudio
import numpy as np
from pydub import AudioSegment, generators
import math

# Initialize PyAudio
p = pyaudio.PyAudio()

mDelayBuffer = [0] * 44100
mWriteIndex = 0

# Parameters
CHUNK_SIZE = 1024
SAMPLE_RATE = 44100
FORMAT = pyaudio.paInt16
CHANNELS = 2

MAX_NUMBER_DELAY_REPEATS = 10
MIN_NUMBER_DELAY_REPEATS = 2

mCurrentNumberDelayRepeats = 10
mCurrentDelayStepSize = 100

mRate = 1
mDepth = 0.3
mDelayRatios = [pow(0.65, i + 1) for i in range(MAX_NUMBER_DELAY_REPEATS)]

a1 = 0
a2 = 0
b0 = 0
b1 = 0
b2 = 0

x1 = 0
x2 = 0
y1 = 0
y2 = 0

user_input = "none"

# Function to update delay buffer
def update_delay_buffer(block):
    global mWriteIndex
    for i in range(CHUNK_SIZE):
        input = block[i]
        mDelayBuffer[mWriteIndex] = input
        if (mWriteIndex == (SAMPLE_RATE - 1)):
            mWriteIndex = 0
        else:
            mWriteIndex += 1
        
def apply_delay(block):
    global mWriteIndex

    for i in range(CHUNK_SIZE):
        input = block[i]
        block[i] = input * mDelayRatios[0]
        currentIndex = mWriteIndex - mCurrentDelayStepSize
        for j in range(1, mCurrentNumberDelayRepeats):
            while currentIndex < 0:
                currentIndex += SAMPLE_RATE
            block[i] += mDelayBuffer[currentIndex] * mDelayRatios[j]
            currentIndex -= mCurrentDelayStepSize
            mDelayBuffer[mWriteIndex] = input
            if mWriteIndex == (SAMPLE_RATE - 1):
                mWriteIndex = 0
            else:
                mWriteIndex += 1

def peaking_coefficients(G, fc, Q, fs):
    global b0, b1, b2, a1, a2

    K = math.tan(math.pi * fc / fs)
    V0 = pow(10, G / 20)
    
    if G >= 0:
        b0 = (1 + ((V0 / Q) * K) + pow(K, 2)) / (1 + ((1 / Q) * K) + pow(K, 2))
        b1 = (2 * (pow(K, 2) - 1)) / (1 + ((1 / Q) * K) + pow(K, 2))
        b2 = (1 - ((V0 / Q) * K) + pow(K, 2)) / (1 + ((1 / Q) * K) + pow(K, 2))
        a1 = b1
        a2 = (1 - ((1 / Q) * K) + pow(K, 2)) / (1 + ((1 / Q) * K) + pow(K, 2))
    else:
        b0 = (1 + ((1 / Q) * K) + pow(K, 2)) / (1 + ((V0 / Q) * K) + pow(K, 2))
        b1 = (2 * (pow(K, 2) - 1)) / (1 + ((V0 / Q) * K) + pow(K, 2))
        b2 = (1 - ((1 / Q) * K) + pow(K, 2)) / (1 + ((V0 / Q) * K) + pow(K, 2))
        a1 = b1
        a2 = (1 - ((V0 / Q) * K) + pow(K, 2)) / (1 + ((V0 / Q) * K) + pow(K, 2))
    

def applyBiquad(input, output):
    global x1, x2, y1, y2
    for i in range(CHUNK_SIZE):
        y0 = (b0) * input[i] + (b1) * x1 + (b2) * x2 - (a1) * y1 - (a2) * y2;
        x2 = x1
        x1 = input[i]
        y2 = y1
        y1 = y0
        output[i] = y0
    

# Function to update tremolo effect
def update_tremolo(block, initialWriteIndex):
    for i in range(CHUNK_SIZE):
        sincalc = math.sin(0.00014240362 * initialWriteIndex * mRate)
        factor = (1.0 - mDepth) + mDepth * sincalc
        block[i] *= factor
        if (initialWriteIndex == (SAMPLE_RATE - 1)):
            initialWriteIndex = 0
        else:
            initialWriteIndex += 1

# Function to start audio playback
def start_playback():
    global user_input

    mp3_file = "sunflower.mp3" #input("Please enter your [filename.mp3] file name: ")
    audio_segment = AudioSegment.from_file(mp3_file, format='mp3')
    
    audio_data = np.array(audio_segment.get_array_of_samples())

    # Open audio stream with speaker output
    stream_out = p.open(format=FORMAT,
                        channels=CHANNELS,
                        rate=SAMPLE_RATE,
                        output=True,
                        frames_per_buffer=CHUNK_SIZE,
                        output_device_index=None,
                        output_host_api_specific_stream_info=None)

    print("Playing...")

    while True:
        for i in range(0, len(audio_data), CHUNK_SIZE):
            # Extract chunk of audio data
            chunk = audio_data[i:i + CHUNK_SIZE]

            # Apply effects
            initialWriteIndex = mWriteIndex

            if user_input == "none":
                pass
            elif user_input == "tremolo":
                update_delay_buffer(chunk)
                update_tremolo(chunk, initialWriteIndex)
            elif user_input == "delay":
                apply_delay(chunk)
            elif user_input == "wah":
                input = [0] * CHUNK_SIZE
                output = [0] * CHUNK_SIZE
                for i in range(CHUNK_SIZE):
                    input[i] = float(chunk[i]) / 32768.0

                applyBiquad(input, output)

                for i in range(CHUNK_SIZE):
                    chunk[i] = int(output[i] * 32768.0) * 0.5

            # Convert back to bytes
            output_data = chunk.astype(np.int16).tobytes()

            # Play audio through speaker
            stream_out.write(output_data)

        if stop_flag:
            break

    print("Playback stopped.")

    # Close stream
    stream_out.stop_stream()
    stream_out.close()

# Function to stop playback
def stop_playback():
    global stop_flag
    stop_flag = True

# Create a Tkinter window
window = tk.Tk()
window.title("Audio Effects Control")

# Add a dropdown menu to select the effect
effect_label = ttk.Label(window, text="Select Effect:")
effect_label.grid(row=0, column=0, padx=5, pady=5)

effects = ["None", "Tremolo", "Delay", "Wah"]
effect_selection = ttk.Combobox(window, values=effects)
effect_selection.current(0)  # Set the default selection to "None"
effect_selection.grid(row=0, column=1, padx=5, pady=5)

# Button to start playback
start_button = ttk.Button(window, text="Start Playback", command=start_playback)
start_button.grid(row=1, column=0, padx=5, pady=5)

# Button to stop playback
stop_button = ttk.Button(window, text="Stop Playback", command=stop_playback)
stop_button.grid(row=1, column=1, padx=5, pady=5)

def update_effect():
    global user_input
    user_input = effect_selection.get()

# Button to update effect
update_button = ttk.Button(window, text="Update Effect", command=update_effect)
update_button.grid(row=2, column=0, columnspan=2, padx=5, pady=5)

# Run the Tkinter event loop
window.mainloop()


# Function to update effect based on user selection
def update_effect():
    global user_input
    user_input = effect_selection.get()

# Function to update parameters based on user input
