void setup() {
  Serial.begin(9600);
  tone(7, 440);
  pinMode(11, OUTPUT);
  pinMode(10, INPUT);
  digitalWrite(11, LOW);
}


float prendiVelsuono(float lettura) {
  float T = lettura * (5./1024.) * 100;
  float vs = 331.3 + (0.606 * T);
  return vs;
}

float prendiDistanza(float vS) {
  digitalWrite(11, HIGH);
  delayMicroseconds(10); 
  digitalWrite(11, LOW);
  long durata = pulseIn(10, HIGH);
  return (durata*vS*1.e-6) / 2;
}

void loop() {
  //float fm = analogRead(A1);
  //float vcampione = 0; //da vedere tramite sperimentazione quale è la fluttuazione da fermo
  // vs = (331.3 * sqrt(1 + (T / 273.15))) * 100 / 1000000; formula diversa che ho trovato online
  float lettura1 = analogRead(A0);
  float vs1 = prendiVelsuono(lettura1);
  float d1 = prendiDistanza(vs1);
  delay(200);
  float lettura2 = analogRead(A0);
  float vs2 = prendiVelsuono(lettura2);
  float d2 = prendiDistanza(vs2);
  float vo = (d2 - d1) / 0.2;
  if(d2<0.06)
    noTone(7);
  else
    tone(7,440);
  
  Serial.print("d1:");
  Serial.println(d1);
  Serial.println(" ");

  Serial.print("d2:");
  Serial.println(d2);
  Serial.println(" ");
  
  Serial.print("vo:");
  Serial.println(vo);
  Serial.println(" ");

  //calcolo effetto doppler aspettato
  //if(vo>vcampione){
  //float fd = 440*(vs/(vs-vo));
  //Serial.print("frequenza shiftata: "); Serial.println(fd);
  //.print("frequenza misurata: "); Serial.println(fm);
//}
}
