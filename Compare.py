import numpy as np
import math
import matplotlib.pyplot as plt
import pandas as pd

data = pd.read_csv("./output/noise-fast-example.csv")
dataNoiseDens = pd.read_csv("./Noise Spectras/noiseSpectra.csv")

dt = data["time"][1] - data["time"][0]

datafft = np.fft.rfft(data["voltage"], norm="ortho")
freq = np.fft.rfftfreq(len(data["voltage"]), d=dt)

V_sqrtHz = np.sqrt(np.abs(datafft)**2 / (freq[-1]-freq[0]))

totalNoise = 0
lastFrequency = 0
lastVoltage = 0
i = 0
for nV, ff in zip(datafft, freq):
	nV = np.abs(nV)**2
	if i == 0:
		lastFrequency = ff
		lastVoltage = nV
		i += 1
		continue

	totalNoise += (ff-lastFrequency)*(nV+lastVoltage)/2
	lastFrequency = ff
	lastVoltage = nV

print("My Simulated Noise standard deviation using the calculated NSD:")
print(np.sqrt(totalNoise/(freq[-1]-freq[0])))
print('\n')

totalNoise = 0
lastFrequency = 0
lastVoltage = 0
i = 0
for ff, nV in zip(dataNoiseDens["frequency"], dataNoiseDens["V(onoise)"]):
	
	if i == 0:
		lastFrequency = ff
		lastVoltage = nV
		i += 1
		continue

	totalNoise += (ff-lastFrequency)*(nV**2+lastVoltage**2)/2
	lastFrequency = ff
	lastVoltage = nV

print("LTSpice Noise standard deviation:")
print(np.sqrt(totalNoise))
print('\n')

plt.figure(1)
plt.subplot(211)
plt.semilogx(freq, V_sqrtHz, dataNoiseDens["frequency"], dataNoiseDens["V(onoise)"])
plt.xlabel("Frequency")
plt.ylabel("Std/Hz^1/2")

plt.subplot(212)
plt.plot(data["time"], data["voltage"])
plt.xlabel("Time")
plt.ylabel("Voltage")


print("Signal mean and std using standard formulas")
print(data.mean())
print(np.sqrt(np.mean(data["voltage"]**2)))

plt.show()

