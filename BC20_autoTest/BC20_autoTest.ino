#include<HardwareSerial.h>
HardwareSerial MySerial(1);

String getValue(String,char,int);
void OpenSocket();

void setup() {
  Serial.begin(9600);
  MySerial.begin(9600,SERIAL_8N1,16,17);
  
  
  MySerial.print("AT+QIOPEN=1,0,\"UDP\",118.27.81.25,8190\r\n");
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
  String cmd="";
  String response="";
  String Latitude="default";
  String Longitude="default";

  MySerial.print("AT+QGNSSRD=\"NMEA/GGA\"\r\n");
  delay(300);

  if(MySerial.available()) {
    response = MySerial.readString();
    response = getValue(response,'$',1);
    response = getValue(response,'\n',0);

    //Serial.println(response);

    Latitude = getValue(response,',',2);
    Longitude = getValue(response,',',4);

    response = "start@4@" + Latitude + "," + Longitude + "@7.1,7.2,7.3,7.4@3.2,3.3,2.2,3.5@4.0,3.4,1.4,4.4@end";
    String temp = "AT+QISEND=0,"+String(response.length())+","+response+"\r\n";
    MySerial.print(temp);
  }
  
  if(Latitude=="" || Longitude=="") {
    MySerial.print("AT+QISEND=0,9,ERROR!!!!\r\n");
    MySerial.readString();
  }

  response="";
  Latitude="default";
  Longitude="default";


  delay(5000);
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



/*
AT+QIOPEN=1,0,"UDP",118.27.81.25,8190
AT+QISTATE?
AT+QISEND=0,8,3B017126
AT+QICLOSE=0
AT+CSQ

AT+QGNSSC?
AT+QGNSSAGPS?


AT+QGNSSRD="NMEA/GGA"
hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
055148,2407.8945,N,12041.7649,E,1,00,1.0,155.2,M,16.6,M,X.X,xxxx,*47
055148 -> UTC
2407.8945,N -> Latitude 
12041.7649,E -> Longitude
1 -> GPS quality indicator (0=invalid; 1=GPS fix; 2=Diff. GPS fix)
00 -> Number of satellites in use
1.0 -> Horizontal dilution of position
155.2 -> Antenna altitude above/below mean sea level (geoid)
M -> Meters
16.6 -> Geoidal separation
M -> Meters
X.X -> Age in seconds since last update from diff. reference station
xxxx -> Diff. reference station ID# 
*47 -> Checksum


AT+QISEND=0,81,start@4@02407.8945,12041.7649@7.1,7.2,7.3,7.4@3.2,3.3,2.2,3.5@4.0,3.4,1.4,4.4@end
*/
