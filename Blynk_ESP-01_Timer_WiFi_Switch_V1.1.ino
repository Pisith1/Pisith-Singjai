#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

#define TestLED 2           // on board LED pin assignment GPIO2
  
char auth[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"; // AUTH TOKEN 
char ssid[] = "SINGJAI";    // WiFi Name
char pass[] = "xxxxxxxxxx"; // WiFi Password
char Date[16];
char Time[16]; 
int manual=0;
int oldstatus;
int alldays;
int wifisignal;
long startsecondswd;            
long stopsecondswd;             
long nowseconds;               

BlynkTimer timer;
WidgetRTC rtc;
WidgetTerminal terminal(V3);

void setup()
{
  Serial.begin(9600);
  pinMode(TestLED, OUTPUT);
  digitalWrite(TestLED, LOW); 
  Blynk.begin(auth, ssid, pass);
  rtc.begin();
  timer.setInterval(5000L, activetoday);   
  timer.setInterval(30000L, reconnectBlynk);   
  timer.setInterval(5000L, clockvalue);   
}

void loop()
{
  Blynk.run();
  timer.run();
}


void activetoday(){       
  if(year() != 1970){  
   if (alldays==1) { 
    Blynk.syncVirtual(V8);   
   }
   if (alldays ==2) {
   Blynk.syncVirtual(V16);   
   }
   }
  }

void clockvalue() 
{
 int gmthour = hour();
  if (gmthour == 24){gmthour = 0;}
  String displayhour =   String(gmthour, DEC);
  
  int hourdigits = displayhour.length();
  if(hourdigits == 1){displayhour = "0" + displayhour;}
  
  String displayminute = String(minute(), DEC);
  int minutedigits = displayminute.length();  
  if(minutedigits == 1){displayminute = "0" + displayminute;}
  wifisignal = map(WiFi.RSSI(), -105, -40, 0, 100);
  Blynk.setProperty(V3, "label", "TIME:  " + displayhour + ":" + displayminute + "          WiFi Signal:  " + wifisignal +" %");
}


void reconnectBlynk() {
  if (!Blynk.connected()) {
     if(Blynk.connect()) {
     BLYNK_LOG("Reconnected");
     } else {
     BLYNK_LOG("Not reconnected");
    }
  }
}


BLYNK_WRITE(V2)   
{
    if (param.asInt()==1) {    
    terminal.println();
    terminal.println();
    terminal.println("Manual MODE is ON");
    terminal.println("Press ON/OFF button if required");
    terminal.println("Device is ON");
    terminal.println();
    terminal.println();
    terminal.flush();
    
        if (manual==0){       
         manual=1;
         alldays=0;
         Blynk.virtualWrite(V9, 0);
         digitalWrite(TestLED, HIGH);  
         Blynk.virtualWrite(V2, 1);   
         
         } else {              
         alldays=0;
         Blynk.virtualWrite(V9, 0);
         digitalWrite(TestLED, HIGH);  
         Blynk.virtualWrite(V2, 1);   
         }                           
    }else {
      
    terminal.println();
    terminal.println();
    terminal.println("Manual MODE is ON");
    terminal.println("Press ON/OFF button if required");
    terminal.println("Device is OFF");
    terminal.println();
    terminal.println();
    terminal.flush();

        if (manual==0){      
         manual=1;
         alldays=0;
         Blynk.virtualWrite(V9, 0);
         digitalWrite(TestLED, LOW); 
         Blynk.virtualWrite(V2, 0);   
         } else {  
         alldays=0;
         Blynk.virtualWrite(V9, 0);
         digitalWrite(TestLED, LOW); 
         Blynk.virtualWrite(V2, 0);   
         }  
         }
}


BLYNK_WRITE(V8)
{  
  if (alldays==1) {         
    sprintf(Date, "%02d/%02d/%04d",  day(), month(), year());
    sprintf(Time, "%02d:%02d:%02d", hour(), minute(), second());
    TimeInputParam t(param);
    terminal.print("Timer Checked schedule at: ");
    terminal.println(Time);
    terminal.flush();
    int dayadjustment = -1;  
    if(weekday() == 1){dayadjustment =  6;}
    if(t.isWeekdaySelected(weekday() + dayadjustment)){ 
    terminal.println("TIMER 1 is selected today");
    terminal.println();
    terminal.flush();
    
     for (int i = 1; i <= 7; i++) {  
        if (t.isWeekdaySelected(i)) {
        String d = " ";
        if (i == 1){d = "Monday   ";}
        if (i == 2){d = "Tuesday  ";} 
        if (i == 3){d = "Wednesday";} 
        if (i == 4){d = "Thursday ";} 
        if (i == 5){d = "Friday   ";} 
        if (i == 6){d = "Saturday ";} 
        if (i == 7){d = "Sunday   ";}        
        terminal.println(d + " is selected");
        terminal.flush();
        }
      } 

    if (t.hasStartTime()) 
    {
        String displayhour =   String(t.getStartHour(), DEC);
        int hourdigits = displayhour.length();
        if(hourdigits == 1){displayhour = "0" + displayhour;}
        String displayminute = String(t.getStartMinute(), DEC);
        int minutedigits = displayminute.length();  
        if(minutedigits == 1){displayminute = "0" + displayminute;}
  
      terminal.println(String("Start: ") + displayhour + ":" + displayminute);
      terminal.flush();
    }
    if (t.hasStopTime()) 
    {
        String displayhour =   String(t.getStopHour(), DEC);
        int hourdigits = displayhour.length();
        if(hourdigits == 1){displayhour = "0" + displayhour;}
        String displayminute = String(t.getStopMinute(), DEC);
        int minutedigits = displayminute.length();  
        if(minutedigits == 1){displayminute = "0" + displayminute;}
      
      terminal.println(String("Stop : ") + displayhour + ":" + displayminute);
      terminal.flush();
    }
    terminal.println(String("Time zone: ") + t.getTZ());  
    terminal.flush();
     
    nowseconds = ((hour() * 3600) + (minute() * 60) + second());
    startsecondswd = (t.getStartHour() * 3600) + (t.getStartMinute() * 60);
    
    if(nowseconds >= startsecondswd){    
    }
    else{
      terminal.println("Device is OFF");
      terminal.flush();
      alldays = 2;     
    }
    stopsecondswd = (t.getStopHour() * 3600) + (t.getStopMinute() * 60);
    if(nowseconds >= stopsecondswd){
      if (stopsecondswd >= 86340) {delay (60000);}
      digitalWrite(TestLED, LOW); 
      Blynk.virtualWrite(V2, 0);
      terminal.println("Device is OFF");
      terminal.flush();
      alldays = 2;           
    }
    else{
      if(nowseconds >= startsecondswd){  
        digitalWrite(TestLED, HIGH); 
        Blynk.virtualWrite(V2, 1);
        terminal.println("Device is ON");
        terminal.flush();
      }          
    }
  }
  else{
    terminal.println("TIMER 1 is not selected today");
    terminal.flush();
    alldays = 2;  
  }
  terminal.println();
}
}


BLYNK_WRITE(V9){
  if (param.asInt()==1) {
   pinMode(TestLED, OUTPUT);
   digitalWrite(TestLED, LOW); 
   Blynk.virtualWrite(V2, 0);
   terminal.println();
   terminal.println();
   terminal.println("TIMER is ON");
   terminal.println("Please wait 5 seconds for update");
   terminal.println();
   terminal.println();
   terminal.flush();
   alldays=1;
  } else {
    terminal.println();
    terminal.println();
    terminal.println("Timer is OFF");
    terminal.println();
    terminal.println();
    terminal.flush();
    alldays=0;
}
}



BLYNK_WRITE(V16)
{  
  if (alldays==2) {         
    sprintf(Date, "%02d/%02d/%04d",  day(), month(), year());
    sprintf(Time, "%02d:%02d:%02d", hour(), minute(), second());
    TimeInputParam t(param);
    terminal.print("Timer Checked schedule at: ");
    terminal.println(Time);
    terminal.flush();
    int dayadjustment = -1;  
    if(weekday() == 1){dayadjustment =  6;}
    if(t.isWeekdaySelected(weekday() + dayadjustment)){ 
    terminal.println("TIMER 2 is selected today");
    terminal.println();
    terminal.flush();
    
     for (int i = 1; i <= 7; i++) {  
        if (t.isWeekdaySelected(i)) {
        String d = " ";
        if (i == 1){d = "Monday   ";}
        if (i == 2){d = "Tuesday  ";} 
        if (i == 3){d = "Wednesday";} 
        if (i == 4){d = "Thursday ";} 
        if (i == 5){d = "Friday   ";} 
        if (i == 6){d = "Saturday ";} 
        if (i == 7){d = "Sunday   ";}        
        terminal.println(d + " is selected");
        terminal.flush();
        }
      } 

    if (t.hasStartTime()) 
    {
        String displayhour =   String(t.getStartHour(), DEC);
        int hourdigits = displayhour.length();
        if(hourdigits == 1){displayhour = "0" + displayhour;}
        String displayminute = String(t.getStartMinute(), DEC);
        int minutedigits = displayminute.length();  
        if(minutedigits == 1){displayminute = "0" + displayminute;}
  
      terminal.println(String("Start: ") + displayhour + ":" + displayminute);
      terminal.flush();
    }
    if (t.hasStopTime()) 
    {
        String displayhour =   String(t.getStopHour(), DEC);
        int hourdigits = displayhour.length();
        if(hourdigits == 1){displayhour = "0" + displayhour;}
        String displayminute = String(t.getStopMinute(), DEC);
        int minutedigits = displayminute.length();  
        if(minutedigits == 1){displayminute = "0" + displayminute;}
      
      terminal.println(String("Stop : ") + displayhour + ":" + displayminute);
      terminal.flush();
    }
    terminal.println(String("Time zone: ") + t.getTZ());  
    terminal.flush();
     
    nowseconds = ((hour() * 3600) + (minute() * 60) + second());
    startsecondswd = (t.getStartHour() * 3600) + (t.getStartMinute() * 60);
    
    if(nowseconds >= startsecondswd){    
    }
    else{
      terminal.println("Device is OFF");
      terminal.flush();
      alldays = 1;     
    }
    stopsecondswd = (t.getStopHour() * 3600) + (t.getStopMinute() * 60);
    if(nowseconds >= stopsecondswd){
      if (stopsecondswd >= 86340) {delay (60000);}
      digitalWrite(TestLED, LOW); 
      Blynk.virtualWrite(V2, 0);
      terminal.println("Device is OFF");
      terminal.flush();
      alldays = 1;               
    }
    else{
      if(nowseconds >= startsecondswd){  
        digitalWrite(TestLED, HIGH); 
        Blynk.virtualWrite(V2, 1);
        terminal.println("Device is ON");
        terminal.flush();
      }          
    }
  }
  else{
    terminal.println("TIMER 2 is not selected today");
    terminal.flush();
    alldays = 1;  
  }
  terminal.println();
}
}



