// 採集資料間距 第幾筆資料

/*

NULL,isTesting,absort,push,stop
是否是測試cmd, 開始下潛後固定此時間會到達指定深度, 在水下待多久 , 推水此時間後會上浮, 等待此時間移動到深度 , 推水到空所需時間, 抽水此時間後下潛
specialCmd  , divingProcessTime              , processTime , controlDepthTime, waitMovingTime     , clearTime      , diveTime

NULL,10,10,3,5,5,10
行為 : 啟動抽水馬達10s > 停機10s > 抽水馬達作動3s推水 > 抽水馬達作動3s吸水  > 抽水馬達作動3s推水 > 抽水馬達作動1s吸水 > 推水10s 

NULL,40,60,10,5,100,25
吸水25s後機體下潛，等待40s到達指定深度，陸續抽推水10s後等待5s(可能超過60s)，推水直到上浮(用大數(100)或是測驗值)

0    1  2  3  4 5  6
NULL,40,60,10,5,32,25
吸水25s後機體下潛，等待40s到達指定深度，陸續抽推水10s後等待5s(可能超過60s)，推水32s後停機(應當無水在機體內)

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
#include<HardwareSerial.h>

HardwareSerial MySerial(1);
const char*ssidAP="ESP32_WiFi";
const char*passwordAP="3B017126";

uint8_t slaveAdd[6] ={0x98,0xD3,0x61,0xF6,0xE1,0xB6};
String Name = "ESP32_Master";
bool connect;
BluetoothSerial BT;

WiFiServer server(80);
String Fullcmd="";

//感測器接腳
int NTUpin = 35;
int TDSpin = 32;
int PHpin  = 34;

String NTU_string = "";
String TDS_string = "";
String PH_string  = "";

int data_counter = 0;

String port = "8190";

String getValue(String,char,int);
float getNTU();
float getTDS();
float getPH();
void collectionData(long int);
String getGPS();

void setup() {
  pinMode(NTUpin,INPUT);
  pinMode(TDSpin,INPUT);
  pinMode(PHpin,INPUT);

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

  MySerial.begin(9600,SERIAL_8N1,16,17);

  MySerial.print("AT+QIOPEN=1,0,\"UDP\",118.27.81.25," + port + "\r\n");
  MySerial.readString();
  delay(300);

  MySerial.print("AT+QGNSSC=1\r\n");
  MySerial.readString();
  delay(300);

  MySerial.print("AT+QGNSSAGPS=1\r\n");
  MySerial.readString();
  delay(300);
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
    long int diveTime;

    if(Fullcmd!="") {

      Serial.println("send Nano: "+Fullcmd); BT.print(Fullcmd);
      client.print("recived");

      if(getValue(Fullcmd, ',', 0) == "isTesting") {
          String temp = "start@test@" + getGPS() + "@end";
          for(int i=0; i<5; i++)  {
            MySerial.print("AT+QISEND=0," + String(temp.length()) +"," + temp + "\r\n");
            delay(500);
          }
      }
      else
      if(getValue(Fullcmd, ',', 0) == "simple") {
        processTime = getValue(Fullcmd, ',', 1).toInt();
        collectionData(processTime);

        NTU_string.remove(NTU_string.length()-1);
        TDS_string.remove(TDS_string.length()-1);
        PH_string.remove(PH_string.length()-1);

        NTU_string = "start@NTU@" + String(data_counter) + "@" + NTU_string + "@end";
        TDS_string = "start@TDS@" + String(data_counter) + "@" + TDS_string + "@end";
        PH_string  = "start@PH@" + String(data_counter) + "@" + PH_string + "@end";
        
        while(true) {
          client = server.available();
          if(client) {
            if(client.available()) break;
          }
          MySerial.print("AT+QISEND=0," + String(NTU_string.length()) + "," + NTU_string + "\r\n");
          Serial.println(String(NTU_string.length()) + "," + NTU_string + "\r\n");
          delay(1000);

          MySerial.print("AT+QISEND=0," + String(TDS_string.length()) + "," + TDS_string + "\r\n");
          Serial.println(String(TDS_string.length()) + "," + TDS_string + "\r\n");
          delay(1000);

          MySerial.print("AT+QISEND=0," + String(PH_string.length()) + "," + PH_string + "\r\n");
          Serial.println(String(PH_string.length()) + "," + PH_string + "\r\n");
          delay(5000);
        }
      }
      if(getValue(Fullcmd, ',', 0) == "NULL") {
        divingProcessTime = getValue(Fullcmd, ',', 1).toInt(); //開始下潛後固定此時間會到達指定深度 
        processTime       = getValue(Fullcmd, ',', 2).toInt(); //在水下待多久
        diveTime          = getValue(Fullcmd, ',', 6).toInt(); //等待此時間後下潛

        delay((diveTime + divingProcessTime) * 1000);
        collectionData(processTime);
        delay(20000);
        
        NTU_string.remove(NTU_string.length()-1);
        TDS_string.remove(TDS_string.length()-1);
        PH_string.remove(PH_string.length()-1);

        NTU_string = "start@NTU@" + String(data_counter) + "@" + NTU_string + "@end";
        TDS_string = "start@TDS@" + String(data_counter) + "@" + TDS_string + "@end";
        PH_string  = "start@PH@" + String(data_counter) + "@" + PH_string + "@end";
        while(true) {
          client = server.available();
          if(client) {
            if(client.available()) break;
          }
          String GPS_string = "start@GPS@" + getGPS() + "@end";
          MySerial.print("AT+QISEND=0," + String(GPS_string.length()) + "," + GPS_string + "\r\n");
          Serial.println(String(GPS_string.length()) + "," + GPS_string + "\r\n");
          delay(1000);

          MySerial.print("AT+QISEND=0," + String(NTU_string.length()) + "," + NTU_string + "\r\n");
          Serial.println(String(NTU_string.length()) + "," + NTU_string + "\r\n");
          delay(1000);

          MySerial.print("AT+QISEND=0," + String(TDS_string.length()) + "," + TDS_string + "\r\n");
          Serial.println(String(TDS_string.length()) + "," + TDS_string + "\r\n");
          delay(1000);

          MySerial.print("AT+QISEND=0," + String(PH_string.length()) + "," + PH_string + "\r\n");
          Serial.println(String(PH_string.length()) + "," + PH_string + "\r\n");
          delay(5000);
        }
      }
      NTU_string   = "";
      TDS_string   = "";
      PH_string    = "";
      data_counter = 0;
      Fullcmd      = "";
    }
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
  long int startTime = millis();
  float avg_NTU = 0.0;
  float avg_TDS = 0.0;
  float avg_PH  = 0.0;
  int counter = 1;

  while(millis() - startTime <= collectionTime * 1000) {
    if(counter <= 5) {
      
      avg_NTU = avg_NTU + (getNTU()/5.0);
      avg_TDS = avg_TDS + (getTDS()/5.0);
      avg_PH  = avg_PH  + (getPH()/5.0);
      
      counter = counter + 1;
      delay(1000); 
    } 
    else {
      counter = 1;
      data_counter = data_counter + 1; 
      NTU_string = NTU_string + String(avg_NTU) + ",";
      TDS_string = TDS_string + String(avg_TDS) + ",";
      PH_string  = PH_string + String(avg_PH)  + ",";

      avg_NTU = 0.0;
      avg_TDS = 0.0;
      avg_PH = 0.0;
    }
  }
}

String getGPS() {
  String response = "";
  MySerial.print("AT+QGNSSRD=\"NMEA/GGA\"\r\n");
  
  while(true){
    if(MySerial.available()) {
      response = MySerial.readString();
      break;
    }
  }
  response = getValue(response,'$', 1);
  response = getValue(response,'\n', 0);
  
  String  Latitude  = getValue(response, ',', 2);
  String  Longitude = getValue(response, ',', 4);
  if(Latitude == "" || Longitude == "") return ("ERROR,ERROR");
  
  return (Latitude + "," + Longitude);
}