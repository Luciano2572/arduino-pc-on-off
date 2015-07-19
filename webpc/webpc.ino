//Release 1.1
/*
 OBS: On here, i am using my relay as HIGH being that it is not being triggered (the pwr on wire). Low triggers it. 
 So it goes LOW - wait 300ms - HIGH to do the trigger
 if your setup is different you gotta alter these
 
*/
#include <SPI.h>
#include <LiquidCrystal.h>
#include <Ethernet.h>
LiquidCrystal lcd(8, 9, 5, 4, 3, 2); 

int buttonState;     
const int buttonPin = 7;
int lastButtonState = LOW;   
long lastDebounceTime = 0; 
long debounceDelay = 30; // Time required for the button to be held down for it to compute it as a press (in ms)
int relay = A1; // Pin the relay is connected to
int pos = 0; 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };   //physical mac address
byte ip[] = { 192, 168, 25, 15 };                      // ip in lan (that's what you need to use in your browser. ("192.168.25.15")
byte gateway[] = { 192, 168, 1, 1 };                   // internet access via router
byte subnet[] = { 255, 255, 255, 0 };                  //subnet mask
EthernetServer server(2525);                             //server port     
int pwrpin = A5;
int boottime = 5;
String readString;
void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(pwrpin, INPUT);
  lcd.begin(16, 2);
  Serial.begin(9600);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH); // This is needed, since my relay board inverts the signal and I just figured that after wiring everything up... Check with your board!
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  boolean pcstatus = ((analogRead(pwrpin) >= 300) ? true : false); // The pwr led from Motherboard is plugged on pin A5 using a 10k resistor to avoid damage, and a 20k resistor pulling down to ground. So when its on, it'll result in a value. From my tests that value is around 400 ish, so >300 will do
  int reading = digitalRead(buttonPin);
  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        //ENTER SHUTDOWN/TURN ON HERE
        if (!pcstatus){
        pcstatus = true;
               digitalWrite(relay, LOW);
               delay(300);
               digitalWrite(relay, HIGH);
               lcd.setCursor(0, 1);
               lcd.print("PC IS BOOTING");
               for(int x = 0; x < boottime; x++){  // Displays the pc is booting msg for 1 minute 
                 lcd.setCursor(0, 1);
                 lcd.print("PC IS BOOTING.  ");
                 delay(250);
                 lcd.setCursor(0, 1);
                 lcd.print("PC IS BOOTING.. ");
                 delay(250);
                 lcd.setCursor(0, 1);
                 lcd.print("PC IS BOOTING...");
                 delay(250);
                 lcd.setCursor(0, 1);
                 lcd.print("PC IS BOOTING   ");
                 delay(250);
                 
                }
                lcd.setCursor(0, 1);
                lcd.print("Power ON OK, LCL"); 
                delay(1000);
      } else {
               digitalWrite(relay, LOW);
               pcstatus = false;
               delay(300);
               digitalWrite(relay, HIGH);
               delay(100);
               lcd.setCursor(0, 1);
               lcd.print("SHUTTING DOWN");
               for(int x = 0; x < boottime; x++){ // Displays the pc is shutting down for 1 minute
                 lcd.setCursor(0, 1);
                 lcd.print("SHUTTING DOWN.  ");
                 delay(250);
                 lcd.setCursor(0, 1);
                 lcd.print("SHUTTING DOWN.. ");
                 delay(250);
                 lcd.setCursor(0, 1);
                 lcd.print("SHUTTING DOWN...");
                 delay(250);
                 lcd.setCursor(0, 1);
                 lcd.print("SHUTTING DOWN   ");
                 delay(250);
                 
                }
                lcd.setCursor(0, 1);
                lcd.print("Shut Down OK,LCL"); 
                delay(1000);
    } 
        
        
      }
    }
  }

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;

  // Create a client connection
  EthernetClient client = server.available();
  if (!pcstatus)
  {
    lcd.setCursor(0, 0);
    lcd.print("PC OFF,SYSTEM OK");

} 


else 
{
  lcd.setCursor(0, 0);
  lcd.print("PC ON, SYSTEM OK");

} 
  if (client) {
    while (client.connected()) {   
      if (client.available()) {
        char c = client.read();
     
        //read char by char HTTP request
        if (readString.length() < 100) {
          //store characters to string
          readString += c;
          //Serial.print(c);
         }

         //if HTTP request has ended
         if (c == '\n') {          
           Serial.println(readString); //print to serial monitor for debuging
     
           client.println("HTTP/1.1 200 OK"); //send new page
           client.println("Content-Type: text/html");
          // client.println("<meta http-equiv=\"refresh\" content=\"1\">");
           client.println();     
           client.println("<HTML>");
           client.println("<HEAD>");
           client.println("<meta name='apple-mobile-web-app-capable' content='yes' />");
           client.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");
           client.println("<link rel='stylesheet' type='text/css' href='http://randomnerdtutorials.com/ethernetcss.css' />");
           client.println("<TITLE>Remote Arduino PC</TITLE>");
           client.println("</HEAD>");
           client.println("<BODY>");
           client.println("<H1>Arduino PC Test</H1>");
           client.println("<hr />");
           client.println("<br />");  
           if (!pcstatus){client.println("<H2>PC status: OFF</H2>");} else {client.println("<H2>PC status: ON</H2>");} 
           client.println("<br />");  
           client.println("<a href=\"/?button1\"\">Switch Status</a>");
          // client.println("<a href=\"/?button1off\"\">Desligar o PC</a><br />");   
           client.println("<br />");     
           client.println("<br />"); 
           client.println("<p>Propriedade de Luciano</p>");  
           client.println("<br />"); 
           client.println("</BODY>");
           client.println("</HTML>");
     
           delay(1);
           //stopping client
           client.stop();
           //controls the Arduino if you press the buttons
           if (readString.indexOf("?button1") >0){
            if (pcstatus = false) 
            {
               pcstatus = true;
               digitalWrite(relay, LOW);
               delay(300);
               digitalWrite(relay, HIGH);
               lcd.setCursor(0, 1);
               lcd.print("PC IS BOOTING");
               for(int x = 0; x < boottime; x++){
                 lcd.setCursor(0, 1);
                 lcd.print("PC IS BOOTING.  ");
                 delay(250);
                 lcd.setCursor(0, 1);
                 lcd.print("PC IS BOOTING.. ");
                 delay(250);
                 lcd.setCursor(0, 1);
                 lcd.print("PC IS BOOTING...");
                 delay(250);
                 lcd.setCursor(0, 1);
                 lcd.print("PC IS BOOTING   ");
                 delay(250);
                 
                }
                lcd.setCursor(0, 1);
                lcd.print("Power ON OK, WEB"); 
                delay(1000);
                        }

               else

               {
               digitalWrite(relay, LOW);
               pcstatus = false;
               delay(300);
               digitalWrite(relay, HIGH);
               delay(100);
               lcd.setCursor(0, 1);
               lcd.print("SHUTTING DOWN");
               for(int x = 0; x < boottime; x++){
                 lcd.setCursor(0, 1);
                 lcd.print("SHUTTING DOWN.  ");
                 delay(250);
                 lcd.setCursor(0, 1);
                 lcd.print("SHUTTING DOWN.. ");
                 delay(250);
                 lcd.setCursor(0, 1);
                 lcd.print("SHUTTING DOWN...");
                 delay(250);
                 lcd.setCursor(0, 1);
                 lcd.print("SHUTTING DOWN   ");
                 delay(250);
                 
                }
                lcd.setCursor(0, 1);
                lcd.print("Shut Down OK,WEB"); 
                delay(1000);
           }
            //clearing string for next read
            readString=""; 
           
         }
         
      }
    }
  }
}
}

