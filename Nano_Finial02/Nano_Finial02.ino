/*

getValue function came from 
https://youyouyou.pixnet.net/blog/post/121477396-arduino%E7%9A%84split%E5%87%BD%E6%95%B8 


做測試得知蓄水槽抽多久會下潛

NULL,isTesting,absort,push
是否是測試cmd, 開始下潛後固定此時間會到達指定深度, 在水下待多久 , 控制深度         , 吸滿所需時間, 推水到空所需時間, 抽水此時間後下潛
specialCmd  , divingProcessTime              , processTime , controlDepthTime, fullTime   , clearTime      , diveTime

*/
#include<SoftwareSerial.h>
#include<string.h>
#include<Servo.h>

Servo white;              //油門信號
Servo blue;               //控制馬達
SoftwareSerial BT(10,11); //HC05
String  Fullcmd = "";     //儲存命令

long int divingProcessTime;        
long int processTime;      
long int controlDepthTime; 
long int fullTime;         
long int clearTime;       
long int diveTime; 

String getValue(String,char,int); //分割命令
void Diving();
void Floating();
void controlDepth();
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
    Fullcmd = BT.readString();
    Serial.print(Fullcmd);
  }

  if(getValue(Fullcmd, ',', 0) == "isTesting") { //開始潛水器測試
    TEST();
  }
  else if(getValue(Fullcmd, ',', 0) == "absort") {  //吸水
    blue.write(0);
    long int sec = getValue(Fullcmd, ',',  1).toInt();
    delay(sec*1000);
  }
  else if(getValue(Fullcmd, ',', 0) == "push") {  //推水
    blue.write(180);
    long int sec = getValue(Fullcmd, ',',  1).toInt();
    delay(sec*1000);    
  }
  else if(getValue(Fullcmd, ',', 0) == "NULL") {  //正式任務
    divingProcessTime = getValue(Fullcmd, ',',  1).toInt(); //開始下潛後固定此時間會到達指定深度
    processTime       = getValue(Fullcmd, ',',  2).toInt(); //在水下待多久
    controlDepthTime  = getValue(Fullcmd, ',',  3).toInt(); //控制深度
    fullTime          = getValue(Fullcmd, ',',  4).toInt(); //吸滿所需時間
    clearTime         = getValue(Fullcmd, ',',  5).toInt(); //推水到空所需時間
    diveTime          = getValue(Fullcmd, ',',  6).toInt(); //抽水到此時間後下潛

    Diving();
  }
  
  Fullcmd = "";
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

void Diving() {

  blue.write(0);
  delay(diveTime * 1000); //執行完後應當下潛，但蓄水槽應未滿
  blue.write(90);


  String ensureStr   = "";  //預設空值，才不會一開始就Time out
  long int startTime = millis();
  int counter        = 1; //用於計算收確認字串的次數MAX = 10

  while(millis() - startTime <= divingProcessTime * 1000) { //開始等待下潛到深度
    if(millis() - startTime <= divingProcessTime * 100 * counter) { //經過特定時間看有沒有收到ensureStr
      if(ensureStr == "fail") { //若是在規定時間後(divingProcessTime / 10)發現ensureStr還是fail
        BT.print("Time out");
        Floating();
        return;
      }
    
      ensureStr = "fail"; //直接預設會Time out
      counter   = counter + 1;
    }
      
    if(BT.available())  ensureStr = BT.readString();  //有收到ensureStr
  } //到達指定深度

  controlDepth(); //控制深度

}

void Floating() {
  blue.write(180);
  delay(clearTime * 1000);
  Fullcmd = "";  //重設Fullcmd,任務完成
}

//不考慮要執行上浮動作時感測器的所在位置
void controlDepth() {
  long int startTime = millis();
  int counter        = 1; 
  String ensureStr   = "";
  bool isDiving      = True;

  while(millis() - startTime <= processTime * 1000) {
    if(millis() - startTime <= processTime * 100 * counter) {
      if(ensureStr == "fail") {
        BT.print("Time out");
        Floating();
        return;
      }
      if(BT.available())  ensureStr = BT.readString(); 
    }
    counter = counter + 1;
    ensureStr = "fail";


    long int delayTime = millis();
    if(millis() - delayTime >= (controlDepthTime *1000) -100) {
      blue.write(90); 
      delay(100);

      if(isDiving) {
        blue.write(180);
        isDiving = Fasle;
      }
      else {
        blue.write(0);
        isDiving = True;
      }
      delayTime = millis();
    }

  }
  Floating(); //開始上浮
}

void TEST() {
  blue.write(0);
  delay(5000);
  blue.write(90);
  
  blue.write(180);
  delay(7000);
  blue.write(90);
  
  Serial.println("Test complete");
  BT.print("Test complete");  //回傳TEST完成，潛水器馬達則可用肉眼看到
}

/*
004 後面測試資料是否要放在作自我測試時順便做紀錄?以人工輸入?可配合不同場景做使用(不同水域密度、黏稠度等因素)，GUI處預測回到水面時間用做測試得值?

081 會需要知道吸滿水的時間嗎?，既然未滿時就會下潛還需要抽滿嗎?為了下潛速度?且固定密度應對於公式計算有正向作用 
 
115 要如何確定說控制深度是有準確度的?在泳池中並無對照物/比例尺  -> 捲尺
*/