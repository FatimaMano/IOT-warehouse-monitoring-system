const int ledpin = 12;  // ledpin,flamepin and buzpin are not changed throughout the process
const int temperaturepin = A2;
const int Flamepin = A0;

const int buzpin = 11;
const int thresholdThermistor = 250;  // sets threshold value for flame sensor
const int thresholdFlame = 150;       // sets threshold value for flame sensor
int flamesensvalue = 0;               // initialize flamesensor reading
int Temperaturesensvalue = 0;         // initialize flamesensor reading
void setup() {
  Serial.begin(9600);
  pinMode(ledpin, OUTPUT);
  pinMode(temperaturepin, INPUT);
  pinMode(Flamepin, INPUT);
  pinMode(buzpin, OUTPUT);
}
void loop() {
  Temperaturesensvalue = analogRead(temperaturepin)*0.48828125;  // reads analog data from flame sensor
  Serial.write(Temperaturesensvalue);
  if (Serial.available() > 0) {
    char receivedChar = Serial.read();
   // Assuming '1' indicates to turn on the buzzer, '0' to turn it off
    if (receivedChar == 'ON') {
      digitalWrite(buzpin, HIGH);
    } else if (receivedChar == 'OFF') {
      digitalWrite(buzpin, LOW);
    }
  }
  flamesensvalue = analogRead(Flamepin);
  if (Temperaturesensvalue <= thresholdThermistor) {  
    Serial.write("Hi!");// compares reading from flame sensor with the threshold value
    digitalWrite(ledpin, HIGH);                       //turns on led and buzzer
  } else {
    digitalWrite(ledpin, LOW);  //turns led off led and buzzer
  }
  if (flamesensvalue <= thresholdFlame) {
    digitalWrite(ledpin, HIGH); 
    tone(buzpin, 100);
  } else {
    noTone(buzpin);
  }
}