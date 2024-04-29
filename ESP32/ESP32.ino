// 採集資料間距 第幾筆資料

/*

NULL,isTesting,absort,push
是否是測試cmd, 開始下潛後固定此時間會到達指定深度, 在水下待多久 , 控制深度         , 吸滿所需時間, 推水到空所需時間, 抽水此時間後下潛
specialCmd  , divingProcessTime              , processTime , controlDepthTime, fullTime   , clearTime      , diveTime

NULL,10,10,3,5,5,10
行為 : 啟動抽水馬達10s > 停機10s > 抽水馬達作動3s推水 > 抽水馬達作動3s吸水  > 抽水馬達作動3s推水 > 抽水馬達作動1s吸水 > 推水10s 

isTesting 
行為 : 發送isTesting給Nano，在發送GPS給server

absort,10
行為 : 抽水10s

push,10
行為 : 推水10s

重複:
  start@GPS@GPS1,GPS2@end
  start@NTU@資料數@NTU,NTU,NTU,NTU@end
  start@TDS@資料數@TDS,TDS,TDS,TDS@end
  start@PH@資料數@PH,PH,PH,PH@end
*/

#include<WiFi.h>
#include<BluetoothSerial.h>

const char*ssidAP="ESP32_WiFi";
const char*passwordAP="3B017126";

uint8_t slaveAdd[6] ={0x98,0xD3,0x61,0xF6,0xE1,0xB6};
String Name = "ESP32_Master";
bool connect;
BluetoothSerial BT;

WiFiServer server(80);
String Fullcmd="";

//感測器接腳
int NTUpin = 12;
int TDSpin = 13;
int PHpin  = 14;

String NTU_string = "";
String TDS_string = "";
String PH_string  = "";


String getValue(String,char,int);
float getNTU();
float getTDS();
float getPH();
void collectionData(long int);

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssidAP,passwordAP);
  delay(500);
  server.begin();

  BT.begin(Name,true);
  connect = BT.connect(slaveAdd);
  if(connect) Serial.println("succ");
  else {
    while(!connect){
      Serial.println("fail");
      connect = BT.connect(slaveAdd);
      delay(5000);
    }
    Serial.println("succ");
  }
}

void loop() {
    WiFiClient client = server.available();
    if(client) {
      Serial.println("Client connect!");
      if(client.available()) {
        Fullcmd=client.readString();
        Serial.println(Fullcmd);
      }
    }
    long int divingProcessTime;
    long int processTime;
    long int collectionTime;
    if(Fullcmd!="") {



      Serial.println("send Nano: "+Fullcmd); BT.print(Fullcmd);
      client.print("recived");
      if(getValue(Fullcmd, ',', 0) == "NULL") {
        divingProcessTime = getValue(Fullcmd, ',',  1).toInt(); //開始下潛後固定此時間會到達指定深度 
        processTime       = getValue(Fullcmd, ',',  2).toInt(); //在水下待多久
        collectionTime = divingProcessTime + processTime; 

        collectionData(collectionTime);
      }
      Fullcmd = "";
    }

    float avg_NTU = 0.0;
    float avg_TDS = 0.0;
    float avg_PH = 0.0;

    int counter = 1;
    long int startTime = millis();

    
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


float getNTU() {
  int sensorValue = analogRead(NTUpin);
  float voltage = sensorValue * (5.0 / 4095.0); 
  return voltage;
}

float getTDS() {
  int sensorValue = analogRead(TDSpin);
  float voltage = sensorValue * (5.0 / 4095.0); 
  return voltage;
}

float getPH() {
  int sensorValue = analogRead(PHpin);
  float voltage = sensorValue * (5.0 / 4095.0); 
  return voltage;
}

void collectionData(long int collectionTime) {

}