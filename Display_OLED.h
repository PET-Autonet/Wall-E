/******************************************************************************************|
|                                                                                          |
|                                                                                          |
|                                                                                          |
|                           FUNÇÃO DO DISPLAY OLED 0.96pol                                 |
|                                                                                          |
|                                                                                          |
|                                                                                          |
/******************************************************************************************/
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define OLED_ADDR 0x3C
int barras[10]; // Alturas das barras (10 barras no total)
int aceleracao = 5; // Velocidade da variação das barras

// Tela quando o Wall-E reproduzir um áudio
void AudioTela() {
  for (int i = 0; i < 10; i++) {
    barras[i] = random(10, SCREEN_HEIGHT);
  }
  display.clearDisplay();
  for (int i = 0; i < 10; i++) {
    int x = i * 12;
    display.fillRect(x, SCREEN_HEIGHT - barras[i], 10, barras[i], SSD1306_WHITE);
  }
  display.display();
  for (int i = 0; i < 10; i++) {
    barras[i] += random(-aceleracao, aceleracao);
    if (barras[i] < 5) {
      barras[i] = 5;
    } else if (barras[i] > SCREEN_HEIGHT) {
      barras[i] = SCREEN_HEIGHT;
    }
  }
}

// Função que exibe a tela inicial
void TelaInicial(){
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(10, 0);
  display.println("Angelo Arthur Joao");
  
  display.setTextSize(3);
  display.setCursor(36, 16);
  display.println("PET");
  
  display.setTextSize(3);
  display.setCursor(0, 40);
  display.println("AUTONET");

  display.display();
}