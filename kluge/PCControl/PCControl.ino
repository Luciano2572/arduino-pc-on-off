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
byte mac[] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
}; //physical mac address
byte ip[] = {
    192, 168, 25, 178
}; // ip in lan (that's what you need to use in your browser. ("192.168.25.15")
byte gateway[] = {
    192, 168, 1, 1
}; // internet access via router
byte subnet[] = {
    255, 255, 255, 0
}; //subnet mask
EthernetServer server(80); //server port     
int pwrpin = A5;
int boottime = 5;
//void toggleState();
bool pcstatus;
bool checkKey();

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
    pcstatus = (analogRead(pwrpin) >= 300); // The pwr led from Motherboard is plugged on pin A5 using a 10k resistor to avoid damage, and a 20k resistor pulling down to ground. So when its on, it'll result in a value. From my tests that value is around 400 ish, so >300 will do
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
                if (!pcstatus) {
                    pcstatus = true;
                    digitalWrite(relay, LOW);
                    delay(300);
                    digitalWrite(relay, HIGH);
                    lcd.setCursor(0, 1);
                    lcd.print("PC IS BOOTING");
                    for (int x = 0; x < boottime; x++) { // Displays the pc is booting msg for 1 minute 
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
                    for (int x = 0; x < boottime; x++) { // Displays the pc is shutting down for 1 minute
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
                    lcd.print("Shut Down OK, LCL");
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
    if (!pcstatus) {
        lcd.setCursor(0, 0);
        lcd.print("PC OFF,SYSTEM OK");

    } else {
        lcd.setCursor(0, 0);
        lcd.print("PC ON, SYSTEM OK");

    }
    if (client) {
      String readString;
        while (client.connected()) {
            if (client.available()) {

              //this should also contain the recieved POST data as well as the GET data
              char c = client.read();
                readString += c;

                //if HTTP request has ended
                if (c == '\n') {
                  Serial.print("readString: ");
                    Serial.println(readString); //print to serial monitor for debuging

                    //send a default HTTP200 response
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println();

                    //controls the Arduino if you press the buttons
                    //if button pressed, handle button and print a response (success or fail, on/off)
                    if (readString.indexOf("toggleState") > 0) {
                        // GET ?toggleState=toggleState&key=password64 HTTP/1.1
                        String queryString = readString.substring(readString.indexOf('?'), readString.indexOf(' ', 5)); //?toggleState=toggleState&key=password64
                        int ind1 = queryString.indexOf('&') + 4; //&key=
                        String pass64 = queryString.substring(ind1); //password64
                        Serial.print("queryString: ");
                        Serial.println(queryString);
                        Serial.print("pass64: ");
                        Serial.println(pass64);
                        
                        if (checkKey(pass64)) {
      if (!pcstatus) {
      pcstatus = true;
      digitalWrite(relay, LOW);
      delay(300);
      digitalWrite(relay, HIGH);
      lcd.setCursor(0, 1);
      lcd.print("PC IS BOOTING");
      for (int x = 0; x < boottime; x++) {
        lcd.setCursor(0, 1);
          lcd.print("PC IS BOOTING.  ");
          delay(1000);
      }
      lcd.setCursor(0, 1);
      lcd.print("Power ON OK, WEB");
      delay(1000);
      } else {
      digitalWrite(relay, LOW);
      pcstatus = false;
      delay(300);
      digitalWrite(relay, HIGH);
      delay(100);
      lcd.setCursor(0, 1);
      lcd.print("SHUTTING DOWN");
      for (int x = 0; x < boottime; x++) {
          lcd.setCursor(0, 1);
          lcd.print("SHUTTING DOWN.  ");
          delay(1000);
      }
      lcd.setCursor(0, 1);
      lcd.print("Shut Down OK, WEB");
      delay(1000);
  }
                            client.println("1");
                        } else {
                            client.println("Not Authorized");
                        }
                      //clearing string for next read
                    } else if (readString.indexOf("getStatus") > 0) {
                      Serial.println("Recieved status request");
                      client.println((pcstatus) ? "1" : "0");
                    } else {
                      Serial.println("serving page");
                      client.println("<html>");
                      client.println("<head>");
                      client.println("<meta name='apple-mobile-web-app-capable' content='yes' />");
                      client.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");
                      client.println("<link rel='stylesheet' type='text/css' href='http://lucianoalberto.zapto.org/arduino/arduino.css' />");
                      client.println("<link rel='stylesheet' type='text/css' href='http://localhost/tests/sig/arduino.css' />");
                      client.println("<script src='https://code.jquery.com/jquery-2.1.4.min.js'></script>");
                      client.print("<script>$.getScript('//' + window.location.hostname + '/arduino/arduinojs.js')");
                      client.println(".fail(function() {console.warn('loading alternate script');$.getScript('http://lucianoalberto.zapto.org/arduino/arduinojs.js')});</script>");
                      client.println("<title>Remote Arduino PC</title>");
                      client.println("</head>");
                      client.println("<body>");
                      client.println("<h1 class='title'>Arduino PC Remote</h1>");
                      client.println("<div class='content'>");
                      client.println("<span class='result' id='statusResult'>Fetching...</span>");
                      client.println("<input type='password' id='pass' placeholder='password' onblur='validate()' autofocus />");
                      client.println("<button class='btn btn-toggle' id='btn-toggle' onclick='sendToggle()'>Switch State</button>");
                      client.println("<img class='logo' src='https://upload.wikimedia.org/wikipedia/commons/8/87/Arduino_Logo.svg' alt='arduino logo' />");
                      client.println("</div>");
                      client.println("</body>");
                      client.println("</html>");

                      delay(1);
                  }
                  readString = "";
                  client.stop();                   
                }
            }
        }
    }
}
/*
void toggleState() {
  //controls the Arduino if you press the buttons
  if (!pcstatus) {
      pcstatus = true;
      digitalWrite(relay, LOW);
      delay(300);
      digitalWrite(relay, HIGH);
      lcd.setCursor(0, 1);
      lcd.print("PC IS BOOTING");
      for (int x = 0; x < boottime; x++) {
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
  } else {
      digitalWrite(relay, LOW);
      pcstatus = false;
      delay(300);
      digitalWrite(relay, HIGH);
      delay(100);
      lcd.setCursor(0, 1);
      lcd.print("SHUTTING DOWN");
      for (int x = 0; x < boottime; x++) {
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
      lcd.print("Shut Down OK, WEB");
      delay(1000);
  }
}*/
bool checkKey(String key) {
    return key == "cGFzc3dvcmQ=";
}
