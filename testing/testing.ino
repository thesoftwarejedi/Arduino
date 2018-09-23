void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("start");
  pinMode(A4, OUTPUT);
  digitalWrite(A4, LOW);
  pinMode(A6, INPUT);
}

void loop() {
  Serial.println(analogRead(A6));
  delay(100);
}
