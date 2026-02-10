''' SPETTRO IN FREQUENZA DI UNA REGISTRAZIONE '''
import numpy as np
import matplotlib.pyplot as plt
import soundcard as sc
from scipy import fft

#Accesso ai dispositivi del computer
speakers = sc.all_speakers()
default_speaker = sc.default_speaker()
mics=sc.all_microphones()
default_mic=sc.default_microphone()

#Lettura dei file .wav
samplerate2=44100
n=int(input('Digitare la durata della registrazione in secondi: '))
print('Inizio prima regitrazione')
data2=default_mic.record(samplerate=samplerate2, numframes=samplerate2*n)
print('Fine prima registrazione')
m=int(input('Fai partire la seconda registrazione '))
print('Inizio seconda regitrazione')
data3=default_mic.record(samplerate=samplerate2, numframes=samplerate2*n)
print('Fine seconda registrazione')
#Estrazione dei dati del wav
y2=data2[:,0]
y3=data3[:,0]
num2=len(y2)
t2=np.linspace(0 , num2/samplerate2, num2)

#Realizzazione del grafico: waveform
plt.subplots(figsize=(11,7))
plt.plot(t2, y2, color='coral')
plt.plot(t2, y3, color='coral')
plt.title('Waveform', size=30)
plt.xlabel('Tempo (s)', size=20)
plt.ylabel('Ampiezza (UA)', size=20)
plt.show()

''' ANALISI DI FOURIER '''
#Trasformata e normalizzazione
c2=fft.fft(y2)
freq2=fft.fftfreq(len(c2), 1.0/samplerate2)
r2=c2.real #parte reale
i2=c2.imag #parte immaginaria
p2=np.abs(c2)**2 #potenza

c3=fft.fft(y3)
freq3=fft.fftfreq(len(c3), 1.0/samplerate2)
r3=c3.real #parte reale
i3=c3.imag #parte immaginaria
p3=np.abs(c3)**2 #potenza

p2_0=max(p2) #normalizzazione
p2_norm=p2/p2_0
p3_0=max(p3) #normalizzazione
p3_norm=p3/p3_0

plt.figure()
plt.title('Prova strumento: Spettro di potenza normalizzato', fontsize=25)
plt.plot(freq2[:len(freq2)//2], p2_norm[:len(p2_norm)//2], color='coral', marker='*')
plt.plot(freq3[:len(freq3)//2], p3_norm[:len(p3_norm)//2], color='blue', marker='*')
plt.grid(linestyle=':')
plt.xlabel('Frequenza (Hz)', fontsize=20)
plt.ylabel('|C_k|^2 norm', fontsize=20)
plt.savefig('Spettro_potenza_chitarra.png', bbox_inches='tight')
plt.show()
