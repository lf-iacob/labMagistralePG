void setup() {
  pinMode(7,HIGH);
  Serial.begin(9600);
  pinMode(10, OUTPUT);
  pinMode(9, INPUT);
  digitalWrite(10, LOW);
}

void loop() {
    digitalWrite(10, HIGH);
    delayMicroseconds(10);
    digitalWrite(10, LOW);
    unsigned long t = pulseIn(9, HIGH);
    float d = (343.*t*1e-6)/2;
    if(d<0.1)
      digitalWrite(7, HIGH);
    else
      digitalWrite(7,LOW);
    Serial.println(d);
    delay(50);
}
