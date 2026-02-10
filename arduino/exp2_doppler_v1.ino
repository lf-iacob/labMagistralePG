void setup() {
  Serial.begin(9600); //connection between board and PC
  tone(7, 440); //speaker
  pinMode(9, OUTPUT); //trigger ultrasound sensor
  pinMode(10, INPUT); //echo ultrasound sensor
  pinMode(6, INPUT); //microphone
}

float prendiDistanza(float vS) { //measures distance
  digitalWrite(9, LOW);
  delayMicroseconds(2);
  digitalWrite(9, HIGH);
  delayMicroseconds(10); 
  digitalWrite(9, LOW);
  long durata = pulseIn(10, HIGH);
  return (durata * vS*1e-6) / 2;
}

void loop() {
  float fm = analogRead(A1); //microphone
  float vcampione = 0; //da vedere tramite sperimentazione quale è la fluttuazione da fermo
  float lettura = analogRead(A0);
  float voltaggio = lettura * (5.0 / 1024.0);
  float T = voltaggio * 100.0;
  float vs = 331.3 + (0.606 * T);
  // vs = (331.3 * sqrt(1 + (T / 273.15))) * 100 / 1000000; formula diversa che ho trovato online
  float d1 = prendiDistanza(vs);
  if(d1<0.06)
    noTone(7);
  else
    tone(7,440);
  delay(100);
  float d2 = prendiDistanza(vs);
  float vo = (d2 - d1) * 10;
  Serial.print("Temp: "); Serial.println(T);
  Serial.print(" Velocita Oggetto: "); 
  Serial.print(vo);
  //calcolo effetto doppler aspettato
  if(vo>vcampione){
  float fd = 440*(vs/(vs-vo));
  Serial.print("frequenza shiftata: "); Serial.println(fd);
  Serial.print("frequenza misurata: "); Serial.println(fm);
}
}
