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

void setup() {
  Serial.begin(19200);  //Serial測試
  BT.begin(9600);       
  white.attach(5);      
  blue.attach(6);

  blue.write(90);       //先預設馬達停止

  delay(500);
}
void loop() {
  blue.write(0);
  delay(20*1000);
/*
  blue.write(90);
  delay(200);
  
  blue.write(180);
  delay(22*1000);
  
  blue.write(90);
  delay(5000);
  */  
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

