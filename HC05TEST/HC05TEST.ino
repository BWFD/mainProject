/*

getValue function came from 
https://youyouyou.pixnet.net/blog/post/121477396-arduino%E7%9A%84split%E5%87%BD%E6%95%B8 


做測試得知蓄水槽抽多久會下潛

NULL,isTesting,absort,push
是否是測試cmd, 開始下潛後固定此時間會到達指定深度, 在水下待多久 , 控制深度         , 吸滿所需時間, 推水到空所需時間, 抽水此時間後下潛
specialCmd  , divingProcessTime              , processTime , controlDepthTime, fullTime   , clearTime      , diveTime

*/
#include<SoftwareSerial.h>

SoftwareSerial BT(10,11); //HC05
int statePin = 12;       //是否有連線



void setup() {
  Serial.begin(19200);  //Serial測試
  BT.begin(9600);      
  pinMode(statePin,INPUT);
}

void loop() {
  Serial.println(digitalRead(statePin));
  delay(500);
}

