
''' ------- FREQUENCY SPECTRUM OF A RECORDING ------- '''
import numpy as np
import matplotlib.pyplot as plt
import soundcard as sc
from scipy import fft

''' --- Recording process --- '''
# Access to the computer speakers
speakers = sc.all_speakers()
default_speaker = sc.default_speaker()
mics=sc.all_microphones()
default_mic=sc.default_microphone()

# Read file .wav
samplerate=44100
n=int(input('Duration of the recordings (s): '))
print(' --- Object at rest: vo = 0 m/s --- ')
print('START FIRST RECORDING')
data1=default_mic.record(samplerate=samplerate, numframes=samplerate*n)
print('END FIRST RECORDING')
m=int(input('Write any number to start the second recording ---> '))
print(' --- Object in motion: vo --- ')
print('START SECOND RECORDING')
data2=default_mic.record(samplerate=samplerate, numframes=samplerate*n)
print('END SECOND RECORDING')

# Data extraction
y1=data1[:,0]
y2=data2[:,0]
num=len(y1)
t=np.linspace(0 , num/samplerate, num)

# Waveform
plt.subplots(figsize=(11,7))
plt.plot(t2, y2, color='coral')
plt.plot(t2, y3, color='blue')
plt.title('Waveform', size=30)
plt.xlabel('Time (s)', size=20)
plt.ylabel('Amplitude (a.u.)', size=20)
plt.show()


''' --- Fourier Analysis --- '''
# Fourier transform
c1=fft.fft(y1)
freq1=fft.fftfreq(len(c1), 1.0/samplerate)
p1=np.abs(c1)**2  #power
p1_0=max(p1)      #normalisation to the max value
p1_norm=p1/p1_0

c2=fft.fft(y2)
freq2=fft.fftfreq(len(c2), 1.0/samplerate)
p2=np.abs(c2)**2
p2_0=max(p2)
p2_norm=p2/p2_0

# Power spectrum
plt.figure()
plt.title('Normalised power spectrum')
plt.plot(freq1[:len(freq1)//2], p1_norm[:len(p1_norm)//2], color='coral', marker='*')
plt.plot(freq2[:len(freq2)//2], p2_norm[:len(p2_norm)//2], color='blue', marker='*')
plt.grid(linestyle=':')
plt.xlabel('Frequency (Hz)', fontsize=20)
plt.ylabel('|C_k|^2 norm', fontsize=20)
plt.show()
