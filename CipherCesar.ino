#include <LiquidCrystal.h>

// Инициализация LCD дисплея (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

// Переменные для работы с шифром
String inputText = "";
String outputText = "";
int shift = 3; // Сдвиг по умолчанию
bool isReady = false;

// Функция шифрования/дешифрования Цезаря
String caesarCipher(String text, int shift, bool encrypt) {
  String result = "";
  
  // Если дешифруем, меняем знак сдвига
  if (!encrypt) {
    shift = -shift;
  }
  
  for (int i = 0; i < text.length(); i++) {
    char c = text.charAt(i);
    
    // Обработка заглавных букв
    if (c >= 'A' && c <= 'Z') {
      c = ((c - 'A' + shift + 26) % 26) + 'A';
    }
    // Обработка строчных букв
    else if (c >= 'a' && c <= 'z') {
      c = ((c - 'a' + shift + 26) % 26) + 'a';
    }
    // Обработка русских заглавных букв
    else if (c >= 'А' && c <= 'Я') {
      c = ((c - 'А' + shift + 33) % 33) + 'А';
    }
    // Обработка русских строчных букв
    else if (c >= 'а' && c <= 'я') {
      c = ((c - 'а' + shift + 33) % 33) + 'а';
    }
    // Остальные символы остаются без изменений
    
    result += c;
  }
  
  return result;
}

// Функция отображения текста на LCD с прокруткой
void displayScrollText(String text, int row, int startPos = 0) {
  // Очищаем строку
  lcd.setCursor(0, row);
  lcd.print("                    "); // 20 пробелов
  
  // Отображаем текст с позиции startPos
  lcd.setCursor(0, row);
  String displayText = text.substring(startPos);
  if (displayText.length() > 20) {
    displayText = displayText.substring(0, 20);
  }
  lcd.print(displayText);
}

// Функция обновления дисплея
void updateDisplay() {
  lcd.setCursor(0, 0);
  lcd.print("Shift: ");
  lcd.print(shift);
  lcd.print("           "); // Очистка остатка строки
  
  lcd.setCursor(0, 1);
  lcd.print("Input:");
  lcd.print("              "); // Очистка остатка строки
  
  if (inputText.length() > 0) {
    displayScrollText(inputText, 2);
  } else {
    lcd.setCursor(0, 2);
    lcd.print("                    ");
  }
  
  if (isReady && outputText.length() > 0) {
    displayScrollText(outputText, 3);
  } else {
    lcd.setCursor(0, 3);
    lcd.print("                    ");
  }
}

void setup() {
  // Инициализация LCD дисплея
  lcd.begin(20, 4);
  
  // Инициализация последовательного порта
  Serial.begin(9600);
  
  // Приветственное сообщение
  lcd.setCursor(0, 0);
  lcd.print("Caesar Cipher v1.0");
  lcd.setCursor(0, 1);
  lcd.print("Ready to encode!");
  lcd.setCursor(0, 2);
  lcd.print("Commands:");
  lcd.setCursor(0, 3);
  lcd.print("s<num> e<text> d<text>");
  
  delay(3000);
  
  // Инструкции в Serial Monitor
  Serial.println("=== CAESAR CIPHER CONVERTER ===");
  Serial.println("Commands:");
  Serial.println("s<number> - set shift (e.g., s5)");
  Serial.println("e<text>   - encrypt text (e.g., eHello)");
  Serial.println("d<text>   - decrypt text (e.g., dKhoor)");
  Serial.println("h         - show help");
  Serial.println("c         - clear display");
  Serial.println("==============================");
  
  updateDisplay();
}

void loop() {
  // Проверяем наличие данных в Serial порту
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim(); // Удаляем лишние пробелы
    
    if (command.length() > 0) {
      processCommand(command);
    }
  }
  
  delay(100); // Небольшая задержка для стабильности
}

void processCommand(String command) {
  char cmd = command.charAt(0);
  String data = command.substring(1);
  
  switch (cmd) {
    case 's':
    case 'S':
      // Установка сдвига
      if (data.length() > 0) {
        int newShift = data.toInt();
        if (newShift != 0 || data == "0") {
          shift = newShift;
          Serial.print("Shift set to: ");
          Serial.println(shift);
          isReady = false;
          inputText = "";
          outputText = "";
          updateDisplay();
        } else {
          Serial.println("Error: Invalid shift value");
        }
      }
      break;
      
    case 'e':
    case 'E':
      // Шифрование
      if (data.length() > 0) {
        inputText = data;
        outputText = caesarCipher(inputText, shift, true);
        isReady = true;
        
        Serial.println("=== ENCRYPTION ===");
        Serial.print("Input:  ");
        Serial.println(inputText);
        Serial.print("Output: ");
        Serial.println(outputText);
        Serial.print("Shift:  ");
        Serial.println(shift);
        
        updateDisplay();
      } else {
        Serial.println("Error: No text to encrypt");
      }
      break;
      
    case 'd':
    case 'D':
      // Дешифрование
      if (data.length() > 0) {
        inputText = data;
        outputText = caesarCipher(inputText, shift, false);
        isReady = true;
        
        Serial.println("=== DECRYPTION ===");
        Serial.print("Input:  ");
        Serial.println(inputText);
        Serial.print("Output: ");
        Serial.println(outputText);
        Serial.print("Shift:  ");
        Serial.println(shift);
        
        updateDisplay();
      } else {
        Serial.println("Error: No text to decrypt");
      }
      break;
      
    case 'h':
    case 'H':
      // Помощь
      Serial.println("=== HELP ===");
      Serial.println("s<number> - set shift value");
      Serial.println("e<text>   - encrypt text");
      Serial.println("d<text>   - decrypt text");
      Serial.println("c         - clear display");
      Serial.println("h         - show this help");
      Serial.println("Example: s7, eHello World, dOlssv Dvysk");
      break;
      
    case 'c':
    case 'C':
      // Очистка
      inputText = "";
      outputText = "";
      isReady = false;
      updateDisplay();
      Serial.println("Display cleared");
      break;
      
    default:
      Serial.println("Error: Unknown command. Type 'h' for help");
      break;
  }
}