#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

struct Buffer {
  int pos;
  int8_t* arr;
  size_t capacity;
};

int readingBufferSize = 24;
unsigned long updateInterval = (unsigned long)1000 * (unsigned long)30;
unsigned long readingBufferInterval = (unsigned long)1000 * (unsigned long)60 * (unsigned long)30;
Buffer* readingBuffer;

unsigned long currentMillis = 0;
unsigned long lastMillisUpdate = 0;
unsigned long lastMillisReading = 0;

int8_t low;
int8_t high;

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);

  // Initialize reading buffer
  readingBuffer = newBuffer(readingBufferSize);
  int8_t temp = readTemperature();
  low = temp;
  high = temp;
  for (int i = 0; i < readingBufferSize; i++) {
    insert(readingBuffer, temp);
  }

  updateLCD(temp, temp, temp);
}

void loop() {
  // put your main code here, to run repeatedly:
  currentMillis = millis();
  if (currentMillis - lastMillisUpdate >= updateInterval) {
    lastMillisUpdate = currentMillis;
    int8_t temp = readTemperature();

    if (currentMillis - lastMillisReading >= readingBufferInterval) {
      lastMillisReading = currentMillis;
      insert(readingBuffer, temp);
      minMax(readingBuffer, &low, &high);
    }

    updateLCD(temp, low, high);
  }
}

void updateLCD(int temp, int low, int high) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(temp);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("L:");
  lcd.print(low);
  lcd.print((char)223);
  lcd.print("C  H:");
  lcd.print(high);
  lcd.print((char)223);
  lcd.print("C");
}

int8_t readTemperature() {
  float voltage = (analogRead(A0) / 1024.0) * 5;
  return (int8_t)((voltage - 0.5) * 100);
}

void minMax(Buffer* buff, int8_t* low, int8_t* high) {
  *low = 127;
  *high = -128;

  for (int i = 0; i < buff->capacity; i++) {
    int8_t value = buff->arr[i];
    if (value < *low) *low = value;
    if (value > *high) *high = value; 
  }
}

Buffer* newBuffer(size_t capacity) {
  Buffer* buff = (Buffer*)malloc(sizeof(Buffer));
  buff->arr = (int8_t*)malloc(capacity * sizeof(int8_t));
  buff->capacity = capacity;
}

void insert(Buffer* buff, int8_t value) {
  buff->pos = (buff->pos + 1) % buff->capacity;
  buff->arr[buff->pos] = value;
}

void freeBuffer(Buffer* buff) {
  free(buff->arr);
  free(buff);
}
