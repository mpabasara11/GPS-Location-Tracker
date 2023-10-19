
boolean waitResponse(String expected_answer="OK", unsigned int timeout=2000);



#include <SoftwareSerial.h>
#include <TinyGPS++.h>



SoftwareSerial SIM900(10,11);
//SoftwareSerial gpsSerial(2, 3);  // RX, TX


TinyGPSPlus gps;


const String APN  = "hutch";


// Variables to store GPS data
double latitude = 0.0;
double longitude = 0.0;

double prevLatitude = 0.0;
double prevLongitude = 0.0;




void setup() {


  Serial.begin(9600);
  SIM900.begin(9600);  
 // gpsSerial.begin(9600);







}


void loop() {


      get_gps();
      
      

  

if (latitude != 0.0 && longitude != 0.0)
{
if(latitude != prevLatitude && longitude != prevLongitude)
{
prevLatitude=latitude;
prevLongitude=longitude;

  Serial.print("Stored latitude = ");
  Serial.print(latitude, 6);
  Serial.print(" Stored longitude = ");
  Serial.println(longitude, 6);


send_data();
//delay(4000);


}

}







}



void get_gps()
{
  while (Serial.available() > 0)
{
  gps.encode(Serial.read());

  if(gps.location.isUpdated())
  {

             latitude = gps.location.lat();
             longitude = gps.location.lng();


    }


  }

  }



void send_data()
{
  
    SIM900.println("AT");
  waitResponse();
  delay(1000);
  
  SIM900.println("AT+CPIN?");
  waitResponse();
  delay(1000);
  
  SIM900.println("AT+CREG?");
  waitResponse();
  delay(1000);
  
  SIM900.println("AT+CGATT?");
  waitResponse();
  delay(1000);
  
  SIM900.println("AT+CIPSHUT");
  waitResponse();
  delay(1000);
  
  SIM900.println("AT+CIPSTATUS");
  waitResponse();
  delay(2000);
  
  SIM900.println("AT+CIPMUX=0");
  waitResponse();
  delay(2000);
  

  SIM900.println("AT+CSTT=\"" + APN + "\"");
  waitResponse();
  delay(1000);
  
  SIM900.println("AT+CIICR");//bring up wireless connection
  waitResponse();
  delay(3000);
  
  SIM900.println("AT+CIFSR");//get local IP adress
  waitResponse();
  delay(2000);
  
  SIM900.println("AT+CIPSPRT=0");
  waitResponse();
  delay(3000);
  
  SIM900.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");//start up the connection
  waitResponse();
  delay(6000); 


  SIM900.println("AT+CIPSEND");//begin send data to remote server
  waitResponse();
  delay(4000);
  
  String str="GET https://api.thingspeak.com/update?api_key=WME6O7X8XQLLUHHJ&field1="+String(latitude,6)+"&field2="+String(longitude,6);
  SIM900.println(str);//begin send data to remote server
  waitResponse();
  delay(4000);
  
  SIM900.println((char)26);//sending
    waitResponse();
  delay(5000);//waitting for reply, important! the time is base on the condition of internet 
  
  SIM900.println("AT+CIPSHUT");//close the connection
    waitResponse();
  delay(100);
  
  
  }


  

//boolean waitResponse(String expected_answer="OK", unsigned int timeout=2000) //uncomment if syntax error (arduino)
boolean waitResponse(String expected_answer, unsigned int timeout) //uncomment if syntax error (esp8266)
{
  uint8_t x=0, answer=0;
  String response;
  unsigned long previous;
    
  //Clean the input buffer
  while( SIM900.available() > 0) SIM900.read();
  

  previous = millis();
  do{
    //if data in UART INPUT BUFFER, reads it
    if(SIM900.available() != 0){
        char c = SIM900.read();
        response.concat(c);
        x++;
        //checks if the (response == expected_answer)
        if(response.indexOf(expected_answer) > 0){
            answer = 1;
        }
    }
  }while((answer == 0) && ((millis() - previous) < timeout));

  
  Serial.println(response);
  return answer;
}
