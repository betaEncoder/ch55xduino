
#define LED_BUILTIN 33

void mDelaymS( uint16_t n );
void int0Callback() {
  USB_CTRL = 0;
  EA = 0;                                                                    //Disabling all interrupts is required.
  mDelaymS( 100 );
  __asm__ ("lcall #0x3800");                                                 //Jump to bootloader code
  while (1);
}

void setup() {
  delay(100);
  
  attachInterrupt(0, int0Callback, FALLING);

  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second

}
