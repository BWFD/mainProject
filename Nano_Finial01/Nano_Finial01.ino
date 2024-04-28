/*
**預設吸滿下潛，空的上浮，雖然不一定
下潛多久需要的時間,在水下待多久,控制深度,吸滿所需時間
sinkSec,underWaterSec,controlSec,defaultSec
*/
#include<SoftwareSerial.h>
#include<string.h>
#include<Servo.h>

Servo white;              //油門信號
Servo blue;               //控制馬達
SoftwareSerial BT(10,11); //HC05
String  Fullcmd="";           //儲存命令
long int Timer=0;     //計算時間

String getValue(String,char,int); //分割命令
void Sink(long int);
void Float(long int);
void Action(long int,long int,long int);
void TEST();

void setup() {
  Serial.begin(19200);  //Serial測試
  BT.begin(9600);       
  white.attach(5);      
  blue.attach(6);

  blue.write(90);       //先預設馬達停止

  delay(500);
}
void loop() {
  if(BT.available()) {
    Fullcmd=BT.readString();
    BT.print("recived");
  }
  if(Fullcmd!="") {
    Serial.println(Fullcmd);
    if(Fullcmd=="test_connect") TEST();
    else {
      long int sinkSec = getValue(Fullcmd,',',0).toInt();
      long int underWaterSec = getValue(Fullcmd,',',1).toInt();
      long int defaultSec= getValue(Fullcmd,',',2).toInt();

      Action(sinkSec,underWaterSec,defaultSec);
    }
  }
  Fullcmd="";
}

String getValue(String data, char separator, int index) { //分割命令
    long int found = 0;
    long int strIndex[] = { 0, -1 };
    long int maxIndex = data.length() - 1;
    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
void Sink(long int sec) {
  unsigned long ensureTime = sec*100  ;
  
  Timer = millis();
  String ensureStr="";

  blue.write(0);

  while(millis()-Timer<=sec*1000) {
    blue.write(0);
    if(BT.available()) {
      ensureStr = BT.readString();
      ensureStr = "*";
      Serial.println("*"+ensureStr+"*");
      ensureStr="";
    }
  }
  blue.write(90);
}
void Float(long int sec) {
  blue.write(180);
  delay(sec*1000);
  blue.write(90);
}
void Action(long int sinkSec,long int underWaterSec,long int defaultSec) {
  Sink(sinkSec);
  Float(sinkSec+2);
} 
void TEST() {
  blue.write(0);
  delay(5000);
  blue.write(90);
  blue.write(180);
  delay(7000);

  Serial.println("Test complete");
  BT.print("Test complete"); //need TEST
    
}
