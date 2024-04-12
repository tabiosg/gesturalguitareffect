import pyaudio
import numpy as np
from pydub import AudioSegment, generators
import math

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

# Generate sine wave segments for each note in C major chord (C, E, G)
c_note = generators.Sine(261.63).to_audio_segment(duration=1000)  # C note (261.63 Hz)
e_note = generators.Sine(329.63).to_audio_segment(duration=1000)  # E note (329.63 Hz)
g_note = generators.Sine(392.00).to_audio_segment(duration=1000)  # G note (392.00 Hz)

# Create C major chord by overlaying the sine wave segments
c_chord = c_note.overlay(e_note).overlay(g_note)

# Repeat the chord for a full minute
full_minute_chord = c_chord * (60 * 1000 // len(c_chord))

# Convert to raw audio data
audio_data = np.array(full_minute_chord.get_array_of_samples())

# Initialize PyAudio
p = pyaudio.PyAudio()

# Open audio stream with speaker output
stream_out = p.open(format=FORMAT,
                    channels=CHANNELS,
                    rate=SAMPLE_RATE,
                    output=True,
                    frames_per_buffer=CHUNK_SIZE,
                    output_device_index=None,
                    output_host_api_specific_stream_info=None)

print("Playing...")

user_input = input("Type in None, Tremolo, Delay, or Wah: ").lower()

if user_input == "none":
    pass
elif user_input == "tremolo":
    mRate = float(input("For rate, enter a float between 1 and 15: "))
    mDepth = float(input("For depth, enter a float between 0 and 0.5: "))
elif user_input == "delay":
    mCurrentNumberDelayRepeats = int(input("For num repeats, enter an integer between 2 and 10: "))
    mCurrentDelayStepSize = int(input("For step size, enter an integer between 100 and " + str(int(SAMPLE_RATE / 10.0)) +": "))
elif user_input == "wah":
    mCurrentGain = float(input("For gain, enter a float between -30 and 30: "))
    mCurrentCenterFrequency = float(input("For center frequency, enter a float between 500 and 5000: "))
    peaking_coefficients(mCurrentGain, mCurrentCenterFrequency, 1.0, 44100.0)
else:
    print("Invalid input. Please type in None, Tremolo, Delay, or Effect.")

try:
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
        
except KeyboardInterrupt:
    print("Playback stopped.")

# Close stream
stream_out.stop_stream()
stream_out.close()
p.terminate()
