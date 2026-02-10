#include <Arduino.h>
#include <arduinoFFT.h>

#define SAMPLES 512
#define SAMPLING_FREQUENCY 4000 // Hz
#define MIC_PIN A1

arduinoFFT FFT = arduinoFFT();
double vReal[SAMPLES];
double vImag[SAMPLES];

void setup() {
  Serial.begin(9600); // connessione tra Arduino e PC
  tone(7, 440);       // speaker
  pinMode(11, OUTPUT); // trigger ultrasuoni
  pinMode(10, INPUT); // echo ultrasuoni
  pinMode(MIC_PIN, INPUT); // microfono
  pinMode(A0, INPUT); // sensore temperatura
}

float prendiDistanza(float vS) { // misura distanza
  digitalWrite(11, LOW);
  delayMicroseconds(2);
  digitalWrite(11, HIGH);
  delayMicroseconds(10); 
  digitalWrite(11, LOW);
  long durata = pulseIn(10, HIGH);
  return (durata * vS * 1e-6) / 2; // distanza in metri
}

void loop() {
  // --- Lettura temperatura e velocità del suono ---
  float lettura = analogRead(A0);
  float voltaggio = lettura * (5.0 / 1024.0);
  float T = voltaggio * 100.0;
  float vs = 331.3 + (0.606 * T); // velocità del suono m/s

  // --- Misura distanza e velocità oggetto ---
  float d1 = prendiDistanza(vs);
  if(d1 < 0.06)
    noTone(7);
  else
    tone(7, 440);

  delay(100);

  float d2 = prendiDistanza(vs);
  float vo = (d2 - d1) * 10; // velocità stimata oggetto
  Serial.print("Temp: "); Serial.println(T);
  Serial.print("Velocita Oggetto: "); Serial.println(vo);

  // --- Calcolo effetto Doppler stimato ---
  float vcampione = 0; // valore di riferimento da fermo
  if(vo > vcampione){
    float fd = 440 * (vs / (vs - vo));
    Serial.print("Frequenza shiftata: "); Serial.println(fd);
  }

  // --- Acquisizione segnale microfono per FFT ---
  for (int i = 0; i < SAMPLES; i++) {
    unsigned long startMicros = micros();
    vReal[i] = analogRead(MIC_PIN);
    vImag[i] = 0;
    while (micros() - startMicros < (1000000.0 / SAMPLING_FREQUENCY));
  }

  // --- Calcolo FFT e spettro di potenza normalizzato ---
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

  double pMax = 0;
  for(int i=0; i<SAMPLES/2; i++){
    vReal[i] = vReal[i] * vReal[i]; // potenza |C_k|^2
    if(vReal[i] > pMax) pMax = vReal[i];
  }

  Serial.println("Frequenza(Hz), Spettro normalizzato");
  for(int i=0; i<SAMPLES/2; i++){
    double freq = (i * 1.0 * SAMPLING_FREQUENCY) / SAMPLES;
    double normalized = vReal[i] / pMax;
    Serial.print(freq);
    Serial.print(",");
    Serial.println(normalized);
  }

  Serial.println("---- Fine ciclo ----\n");
  delay(1000); // pausa prima del prossimo ciclo
}
