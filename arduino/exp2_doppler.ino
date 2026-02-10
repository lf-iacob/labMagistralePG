float f = 1400;
//440
//987.77
//1400
void setup() {
  Serial.begin(9600);
  tone(7, f);
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
  delayMicroseconds(1); //10 
  digitalWrite(11, LOW);
  long durata = pulseIn(10, HIGH);
  return (durata*vS*1.e-6) / 2;
}

void loop() {
  float lettura1 = analogRead(A0);
  float vs1 = prendiVelsuono(lettura1);
  float d1 = prendiDistanza(vs1);
  delay(200);
  float lettura2 = analogRead(A0);
  float vs2 = prendiVelsuono(lettura2);
  float d2 = prendiDistanza(vs2);
  float vo = -(d2 - d1) / 0.2;
  if(d2<0.06)
  {
    noTone(7);
  }
  else
  {
    tone(7,f);
  }

  if(d2<0.06){
    Serial.println("Fine tragitto :)");
    Serial.print("vo:");
    Serial.println(vo);
    float fd = f*(vs2/(vs2-vo));
    Serial.print("fd:"); 
    Serial.println(fd);
    Serial.print(" ");
    f=0;
  }
}
