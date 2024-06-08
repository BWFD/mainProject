void setup() {
  Serial.begin(9600); //Baud rate: 9600
}
void loop() {
  int sensorValue = analogRead(35);// read the input on analog pin 0:
  float voltage = sensorValue * (3.33 / 4095.0); // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  //float ntu = map(voltage, 0, 2.37, 4550, 0);
  float ntu = -1919.83*voltage+4550;
  Serial.print("NTU: "+ String(ntu));
  Serial.println("  voltage: "+String(voltage)); // print out the value you read:
  delay(500);
}
//2.37-0