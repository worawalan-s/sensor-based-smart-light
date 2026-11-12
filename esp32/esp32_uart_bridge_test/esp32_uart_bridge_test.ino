#define UART_RX_PIN 16  // Connect to STM32 PA9 (USART1_TX)
#define UART_TX_PIN 17  // Connect to STM32 PA10 (USART1_RX)

void setup() {
  Serial.begin(115200); // USB Serial Monitor (enable "USB CDC On Boot" if using Native USB port)
  Serial1.begin(115200, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);
  Serial.println("ESP32-S3 UART Bridge Ready");
}

void loop() {
  // 1. Read from STM32, print to Serial Monitor, and reply with PONG on PING
  if (Serial1.available()) {
    String line = Serial1.readStringUntil('\n');
    line.trim();
    if (line.length() > 0) {
      Serial.println("[From STM32] " + line);
      if (line.startsWith("PING:")) {
        Serial1.println("PONG:" + line.substring(5));
      }
    }
  }

  // 2. Forward commands typed in Serial Monitor to STM32 (e.g. CMD:OVERRIDE_ON)
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.length() > 0) {
      Serial1.println(cmd);
      Serial.println("[To STM32] " + cmd);
    }
  }
}
