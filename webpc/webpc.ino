//Release 1.0.a
#include <SPI.h>
#include <LiquidCrystal.h>
#include <Ethernet.h>
#include <Servo.h> 
LiquidCrystal lcd(8, 9, 5, 4, 3, 2); 
int buttonState;     
const int buttonPin = 7;
int lastButtonState = LOW;   
long lastDebounceTime = 0; 
long debounceDelay = 80;
int relay = A1;
boolean pcstatus = false;
Servo microservo; 
int pos = 0; 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };   //physical mac address
byte ip[] = { 192, 168, 25, 15 };                      // ip in lan (that's what you need to use in your browser. ("192.168.1.178")
byte gateway[] = { 192, 168, 1, 1 };                   // internet access via router
byte subnet[] = { 255, 255, 255, 0 };                  //subnet mask
EthernetServer server(2525);                             //server port     
String readString;
void setup() {
  pinMode(buttonPin, INPUT);
  lcd.begin(16, 2);
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  pinMode(relay, OUTPUT);
  microservo.attach(7);
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
// deprecated  if (!pcstatus){digitalWrite (greenled, LOW); digitalWrite(redled, HIGH);} else {digitalWrite(redled, LOW); digitalWrite(greenled, HIGH);} 
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
               digitalWrite(relay, HIGH);
               delay(300);
               digitalWrite(relay, LOW);
               lcd.setCursor(0, 1);
               lcd.print("PC IS BOOTING");
               for(int x = 0; x < 60; x++){
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
      digitalWrite(relay, HIGH);
               pcstatus = false;
               delay(300);
               digitalWrite(relay, LOW);
               delay(100);
               lcd.setCursor(0, 1);
               lcd.print("SHUTTING DOWN");
               for(int x = 0; x < 60; x++){
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
           client.println("<TITLE>Luc's Arduino web</TITLE>");
           client.println("</HEAD>");
           client.println("<BODY>");
           client.println("<H1>Arduino PC Test</H1>");
           client.println("<hr />");
           client.println("<br />");  
           if (!pcstatus){client.println("<H2>PC status: OFF</H2>");} else {client.println("<H2>PC status: ON</H2>");} 
           client.println("<br />");  
           client.println("<a href=\"/?button1on\"\">Ligar o PC</a>");
           client.println("<a href=\"/?button1off\"\">Desligar o PC</a><br />");   
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
           if (readString.indexOf("?button1on") >0){
               pcstatus = true;
               digitalWrite(relay, HIGH);
               delay(300);
               digitalWrite(relay, LOW);
               lcd.setCursor(0, 1);
               lcd.print("PC IS BOOTING");
               for(int x = 0; x < 60; x++){
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
           if (readString.indexOf("?button1off") >0){
               digitalWrite(relay, HIGH);
               pcstatus = false;
               delay(300);
               digitalWrite(relay, LOW);
               delay(100);
               lcd.setCursor(0, 1);
               lcd.print("SHUTTING DOWN");
               for(int x = 0; x < 60; x++){
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

