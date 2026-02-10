// Tested frequencies (Hz): 440, 987.77, 1400

float f = 440;            // frequency originally emitted by the speaker

// setup function
void setup() {
  Serial.begin(9600);     // connection board-PC
  // ultrasound sensor connections
  pinMode(11, OUTPUT);    // trigger digital output pin
  pinMode(10, INPUT);     // echo digital input pin
  digitalWrite(11, LOW);  // triggers starts as 0
}

// function to measure sound speed 
float soundspeed(float lm35_signal) {
  float voltage = lm35_signal * (5./1024.);   // LSB = 5/2^10 V/ADC
  float T =  voltage * 100;                   // temperature conversion factor = 100 °C/V
  float vs = 331.3 + (0.606 * T);             // sound speed depending on the temperature
  return vs;
}

// function to measure the distance with the ultrasound sensor
float distance(float vs) {
  digitalWrite(11, HIGH);
  delayMicroseconds(10); 
  digitalWrite(11, LOW);
  long time = pulseIn(10, HIGH);
  return (time * vs * 1.e-6) / 2;
}

void loop() {
  // first distance
  float signalT1 = analogRead(A0);
  float vs1 = soundspeed(signalT1);
  float d1 = distance(vs1);
  // Δt (0.2s)
  delay(200);
  // second distance
  float signalT2 = analogRead(A0);
  float vs2 = soundspeed(signalT2);
  float d2 = distance(vs2);
  // => object speed (dynamic cart)
  float vo = (d1 - d2)/0.2;

  // Measurement of the doppler effect 
  if(d2<0.06){                        // once the cart reaches the end of the track
    Serial.println("FINISH LINE");
    Serial.print("vo:");
    Serial.println(vo);
    float fd = f*(vs2/(vs2-vo));      // shifted frequency
    Serial.print("fd:"); 
    Serial.println(fd);
    Serial.print(" ");
    f=0;
  }
}
