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
print('Inizio regitrazione')
data2=default_mic.record(samplerate=samplerate2, numframes=samplerate2*n)
print('Fine registrazione')
#Estrazione dei dati del wav
y2=data2[:,0]
num2=len(y2)
t2=np.linspace(0 , num2/samplerate2, num2)

#Realizzazione del grafico: waveform
plt.subplots(figsize=(11,7))
plt.plot(t2, y2, color='coral')
plt.title('Waveform', size=30)
plt.xlabel('Tempo (s)', size=20)
plt.ylabel('Ampiezza (UA)', size=20)
plt.show()

''' ANALISI DI FOURIER '''
#Trasformata e normalizzazione
c2=fft.fft(y2)
freq2=fft.fftfreq(len(c2), d=(t2[1]-t2[0]))
r2=c2.real #parte reale
i2=c2.imag #parte immaginaria
p2=abs(c2)**2 #potenza

fig, ax=plt.subplots(1, 3, figsize=(15,6))
fig.suptitle('Fourier Analysis: Prova strumento', fontsize=35, y=1.05)
ax[0].set_title('Parte reale', fontsize=25)
ax[0].plot(freq2[:len(freq2)//2], r2[:len(freq2)//2], color='coral')
ax[0].grid(linestyle=':')
ax[0].set_xlabel('Frequenza [Hz]', fontsize=20)
ax[0].set_ylabel('Re(C_k) [UA]', fontsize=20)
ax[1].set_title('Parte immaginaria', fontsize=25)
ax[1].plot(freq2[:len(freq2)//2], i2[:len(freq2)//2], color='coral')
ax[1].grid(linestyle=':')
ax[1].set_xlabel('Frequenza [Hz]', fontsize=20)
ax[1].set_ylabel('Im(C_k) [UA]', fontsize=20)
ax[2].set_title('Spettro di potenza', fontsize=25)
ax[2].plot(freq2[:len(freq2)//2], p2[:len(freq2)//2], color='coral')
ax[2].grid(linestyle=':')
ax[2].set_xlabel('Frequenza [Hz]', fontsize=20)
ax[2].set_ylabel('|C_k|^2 [UA]', fontsize=20)
plt.show()

p2_0=max(p2) #normalizzazione
p2_norm=p2/p2_0

plt.figure()
plt.title('Prova strumento: Spettro di potenza normalizzato', fontsize=25)
plt.plot(freq2[:len(freq2)//2], p2_norm[:len(p2_norm)//2], color='coral', marker='*')
plt.grid(linestyle=':')
plt.xlabel('Frequenza (Hz)', fontsize=20)
plt.ylabel('|C_k|^2 norm', fontsize=20)
plt.savefig('Spettro_potenza_chitarra.png', bbox_inches='tight')
plt.show()

