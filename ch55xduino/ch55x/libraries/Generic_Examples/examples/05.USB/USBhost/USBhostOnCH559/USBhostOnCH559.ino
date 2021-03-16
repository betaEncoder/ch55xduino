
#define LED_BUILTIN 17
/*
void InitUSB_Host( void ) {
  //IE_USB = 0;                                                                     //查询方式 CDC still need it!
  USB_CTRL = bUC_HOST_MODE;                                                       // 先设定模式
  USB_DEV_AD = 0x00;
  UH_EP_MOD = bUH_EP_TX_EN | bUH_EP_RX_EN ;                                       //单64字节收发缓冲区
  UH_RX_DMA = RxBuffer;
  UH_TX_DMA = TxBuffer;
  UH_RX_CTRL = 0x00;
  UH_TX_CTRL = 0x00;
  USB_CTRL = bUC_HOST_MODE | bUC_INT_BUSY | bUC_DMA_EN;                           // 启动USB主机及DMA,在中断标志未清除前自动暂停
  UHOST_CTRL = 0;
  UH_SETUP = bUH_SOF_EN;
  USB_INT_FG = 0xFF;                                                              // 清中断标志
  DisableRootHubPort( );                                                          // 清空
  USB_INT_EN = bUIE_TRANSFER | bUIE_DETECT;
}*/

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);


  //initUSB_Host();

}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second

}
