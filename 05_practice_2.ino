#define PIN_LED 7
unsigned int count, toggle;

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_LED,OUTPUT);
  count = 0;
  toggle = 0;
  digitalWrite(PIN_LED,toggle);
  
}

void loop() {
  // put your main code here, to run repeatedly
  digitalWrite(PIN_LED,toggle);
  delay(1000);
  while(count<11){
    toggle = toggle_state(toggle);
    digitalWrite(PIN_LED,toggle);
    count++;
    delay(1000/11);
  }

  while(1){
    
  }
}

int toggle_state(int toggle){
  toggle = !toggle;
  return toggle;
}
