import tkinter as tk
from tkinter import ttk
import pyaudio
import numpy as np
from pydub import AudioSegment, generators
import math
import threading

# Initialize PyAudio
p = pyaudio.PyAudio()

stop_flag = False

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
mDelayRatios = [pow(0.45, i + 1) for i in range(MAX_NUMBER_DELAY_REPEATS)]

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

def peaking_coefficients(G, fc, Q=1.0, fs=44100.0):
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
    

mCurrentGain = 20
mCurrentCenterFrequency = -500

peaking_coefficients(mCurrentGain, mCurrentCenterFrequency, 1.0, 44100.0)

def applyBiquad(input, output):
    global x1, x2, y1, y2
    for i in range(CHUNK_SIZE):
        y0 = (b0) * input[i] + (b1) * x1 + (b2) * x2 - (a1) * y1 - (a2) * y2
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

def start_playback_thread():
    playback_thread = threading.Thread(target=start_playback)
    playback_thread.start()

# Function to start audio playback
def start_playback():
    stop_flag = False
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

            if stop_flag:
                break

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
                    chunk[i] = int(output[i] * 32768.0)

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

# Button to start playback
start_button = ttk.Button(window, text="Start Playback", command=start_playback_thread)
start_button.grid(row=1, column=0, padx=5, pady=5)

# Button to stop playback
stop_button = ttk.Button(window, text="Stop Playback", command=stop_playback)
stop_button.grid(row=1, column=1, padx=5, pady=5)

# Function to update effect based on user selection

# Label for mRate slider
rate_label = ttk.Label(window, text="Tremolo: mRate:")
rate_label.grid(row=3, column=0, padx=5, pady=5)

# Slider to control mRate
rate_slider = ttk.Scale(window, from_=1, to=15, orient=tk.HORIZONTAL, length=200)
rate_slider.set(mRate)
rate_slider.grid(row=3, column=1, padx=5, pady=5)

# Label for mDepth slider
depth_label = ttk.Label(window, text="Tremolo: mDepth:")
depth_label.grid(row=4, column=0, padx=5, pady=5)

# Slider to control mDepth
depth_slider = ttk.Scale(window, from_=0, to=0.5, orient=tk.HORIZONTAL, length=200)
depth_slider.set(mDepth)
depth_slider.grid(row=4, column=1, padx=5, pady=5)

# Label for mCurrentNumberDelayRepeats slider
delay_repeats_label = ttk.Label(window, text="Delay: mCurrentNumberDelayRepeats:")
delay_repeats_label.grid(row=5, column=0, padx=5, pady=5)

# Slider to control mCurrentNumberDelayRepeats
delay_repeats_slider = ttk.Scale(window, from_=2, to=10, orient=tk.HORIZONTAL, length=200)
delay_repeats_slider.set(mCurrentNumberDelayRepeats)
delay_repeats_slider.grid(row=5, column=1, padx=5, pady=5)

# Label for mCurrentDelayStepSize slider
delay_step_label = ttk.Label(window, text="Delay: mCurrentDelayStepSize:")
delay_step_label.grid(row=6, column=0, padx=5, pady=5)

# Slider to control mCurrentDelayStepSize
delay_step_slider = ttk.Scale(window, from_=100, to=int(SAMPLE_RATE / 10.0), orient=tk.HORIZONTAL, length=200)
delay_step_slider.set(mCurrentDelayStepSize)
delay_step_slider.grid(row=6, column=1, padx=5, pady=5)

# Label for mCurrentGain slider
gain_label = ttk.Label(window, text="Wah: mCurrentGain:")
gain_label.grid(row=7, column=0, padx=5, pady=5)

# Slider to control mCurrentGain
gain_slider = ttk.Scale(window, from_=-30, to=30, orient=tk.HORIZONTAL, length=200)
gain_slider.set(mCurrentGain)
gain_slider.grid(row=7, column=1, padx=5, pady=5)

# Label for mCurrentCenterFrequency slider
frequency_label = ttk.Label(window, text="Wah: mCurrentCenterFrequency:")
frequency_label.grid(row=8, column=0, padx=5, pady=5)

# Slider to control mCurrentCenterFrequency
frequency_slider = ttk.Scale(window, from_=500, to=5000, orient=tk.HORIZONTAL, length=200)
frequency_slider.set(mCurrentCenterFrequency)
frequency_slider.grid(row=8, column=1, padx=5, pady=5)

# Function to update mRate variable when its slider is moved
def update_rate(value):
    global mRate
    mRate = int(value)

# Function to update mDepth variable when its slider is moved
def update_depth(value):
    global mDepth
    mDepth = float(value)

# Function to update mCurrentGain variable when its slider is moved
def update_gain(value):
    global mCurrentGain
    mCurrentGain = int(value)

# Function to update mCurrentCenterFrequency variable when its slider is moved
def update_frequency(value):
    global mCurrentCenterFrequency
    mCurrentCenterFrequency = int(value)

# Function to update mCurrentNumberDelayRepeats variable when its slider is moved
def update_delay_repeats(value):
    global mCurrentNumberDelayRepeats
    mCurrentNumberDelayRepeats = int(value)

# Function to update mCurrentDelayStepSize variable when its slider is moved
def update_delay_step(value):
    global mCurrentDelayStepSize
    mCurrentDelayStepSize = int(value)

# Bind functions to slider events
rate_slider.bind("<Motion>", lambda event: update_rate(rate_slider.get()))
depth_slider.bind("<Motion>", lambda event: update_depth(depth_slider.get()))
gain_slider.bind("<Motion>", lambda event: update_gain(gain_slider.get()))
frequency_slider.bind("<Motion>", lambda event: update_frequency(frequency_slider.get()))
delay_repeats_slider.bind("<Motion>", lambda event: update_delay_repeats(delay_repeats_slider.get()))
delay_step_slider.bind("<Motion>", lambda event: update_delay_step(delay_step_slider.get()))

# Function to handle effect button clicks
def handle_effect(effect):
    global user_input
    user_input = effect
    effect_label.config(text=f"Selected Effect: {user_input}")

# Buttons for effect selection
none_button = ttk.Button(window, text="None", command=lambda: handle_effect("none"))
none_button.grid(row=9, column=0, padx=5, pady=5)

tremolo_button = ttk.Button(window, text="Tremolo", command=lambda: handle_effect("tremolo"))
tremolo_button.grid(row=9, column=1, padx=5, pady=5)

delay_button = ttk.Button(window, text="Delay", command=lambda: handle_effect("delay"))
delay_button.grid(row=9, column=2, padx=5, pady=5)

wah_button = ttk.Button(window, text="Wah", command=lambda: handle_effect("wah"))
wah_button.grid(row=9, column=3, padx=5, pady=5)

# Label for selected effect
effect_label = ttk.Label(window, text=f"Selected Effect: {user_input}")
effect_label.grid(row=11, column=0, columnspan=2, padx=5, pady=5)

# Run the Tkinter event loop
window.mainloop()

# Function to update parameters based on user input
