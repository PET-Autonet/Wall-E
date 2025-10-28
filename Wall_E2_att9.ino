// ########################################################################################//

/******************************************************************************************|
|                                                                                          |
|                                                                                          |
|                                                                                          |
|                        by: Angelo; Arthur; João <-> feat: GPT                            |
|                                 Data > 17/10/2024 <                                      |
|                                                                                          |
|                                                                                          |
|                                                                                          |
|******************************************************************************************/

// #########################################################################################//

#include <BluetoothSerial.h>
#include <Arduino.h>
BluetoothSerial SerialBT;
bool dispositivoConectado = false;
const int sizeaudio = 3;

/*####################################### DISPLAY_OLED ################################################*/
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "Display_OLED.h"
/*#####################################################################################################*/

/*####################################### MOTORES #####################################################*/
// Definindo os pinos dos motores
const int IN[] = {32, 33, 25, 26};
int velocidade = 200;  // Velocidade dos movimentos direita e esquerda
int velocidade1 = 0;   // Velocidade dos movimentos frente e trás
int aceleracao1 = 100; // Variável para controlar o quando aumenta a velocidade1
int tipoMovimento = 0; // Flag para variar qual foi o ultimo movimento
int tempomotor;
#include "Motores.h"
/*#####################################################################################################*/

/*####################################### MP3 #########################################################*/
#include <DFPlayerMini_Fast.h>
#include <HardwareSerial.h>
// Definindo o DFPlayer
DFPlayerMini_Fast myMP3;
HardwareSerial mySerial(2); // Usando Serial 2 no ESP32 (GPIO 16 RX2, GPIO 17 TX2)

int currentTrack = 1;   // Música atual
bool isPlaying = false; // Flag para saber se a música está tocando
int volume = 30;        // Volume inicial (0 a 30)
int tempomusic;
#include "MP3.h"
/*#####################################################################################################*/

/*####################################### SERVO-MOTOR #################################################*/
#include <Adafruit_PWMServoDriver.h>

// Configurando o driver PCA9685 na comunicação I2C padrão
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(); // Instanciando objetos com a classe Adafruit_PWMServoDriver.

int graucabeca = 165;     // Variável de controle do grau do servo motor da cabeça (servo do canal 4)
int graupescoco = 0;      // Variável que controla qual movimento o pescoço fará
int olhodireito = 40;     // Variável que controla o angulo do servo motor do olho direito (servo do canal 5)
int olhoesquerdo = 180;   // Variável que controla o angulo do servo motor do olho esquerdo (servo do canal 6)
bool olhocontrol = false; // Variável que controla o tipo de movimento do olho (subida ou descida)

#define MIN_PULSE_WIDTH 500 // Estes são os valores mínimo e máximo de largura de pulso que servem para o MG 995.
#define MAX_PULSE_WIDTH 2500
#define DEFAULT_PULSE_WIDTH 1500
#define FREQUENCY 50

#include "Servos.h"
/*#####################################################################################################*/

void setup()
{
  Serial.begin(115200);
  SerialBT.begin("Wall-E controll");           // Nome do Bluetooth
  Serial.println("O Bluetooth foi iniciado!"); // Informa o usuário que o Bluetooth está conectado

  // Serial.println("16 channel Servo test!");
  pwm.begin();                                // Inicializa a biblioteca e envia sinais PWM.
  pwm.setPWMFreq(FREQUENCY);                  // Frequência de atualização do servo a 60 Hertz.
  pwm.setPWM(0, 0, pulseWidth(180));          // Braço Esquerdo
  pwm.setPWM(1, 0, pulseWidth(0));            // Braço direito
  pwm.setPWM(2, 0, pulseWidth(100));          // Pescoço 1 (não pode ser menor que 100)
  pwm.setPWM(3, 0, pulseWidth(0));            // Pescço 2
  pwm.setPWM(4, 0, pulseWidth(graucabeca));   // Cabeça
  pwm.setPWM(5, 0, pulseWidth(olhodireito));  // Olho Direito (minimo 20) (maximo 100)
  pwm.setPWM(6, 0, pulseWidth(olhoesquerdo)); // Olho Esquerdo (minimo ) (maximo 180)

  // Configurando os pinos dos motores como saída
  for (int i = 0; i < 4; i++)
  {
    pinMode(IN[i], OUTPUT);
  }

  while (!Serial)
    ; // Aguarda o Serial estar pronto (opcional, útil em alguns casos)

  // Inicializa a comunicação com o DFPlayer
  mySerial.begin(9600, SERIAL_8N1, 16, 17); // Configura a comunicação UART com o DFPlayer (pinos corretos)

  if (!myMP3.begin(mySerial))
  {
    Serial.println("Falha ao iniciar DFPlayer Mini");
    while (true)
      ; // Para a execução se o DFPlayer não iniciar
  }

  Serial.println("DFPlayer Mini Iniciado!");

  myMP3.volume(volume); // Configura o volume de 0 a 30
  Serial.println("Aguardando comandos via Bluetooth.");

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR))
  {
    for (;;)
      ;
  }
  TelaInicial();
  // Inicia com os motores desligados
  Parar(velocidade1);
}

void loop()
{
  if (!myMP3.isPlaying())
  {
    TelaInicial();
  }
  if (SerialBT.connected())
  {

    // O dispositivo está conectado.
    // Verificamos se é a *primeira vez* que notamos isso.
    if (!dispositivoConectado)
    {
      Serial.println("Cliente Conectado!");
      dispositivoConectado = true;
      // Coloque aqui qualquer código que deve rodar APENAS QUANDO CONECTA
    }
    if (SerialBT.available())
    {
      char command = SerialBT.read(); // Lê o comando enviado pelo aplicativo
      Serial.print("Comando recebido: ");
      Serial.println(command);
      controlWalle(command); // Chama a função para controlar o carrinho
    }
  }
  else
  {

    // O dispositivo NÃO está conectado.
    // Verificamos se ele *estava* conectado antes, ou seja, se ele acabou de cair.
    if (dispositivoConectado)
    {
      Serial.println("Cliente Desconectado! (Sinal perdido ou conexão fechada)");
      dispositivoConectado = false;
      Parar(velocidade1);
    }
    Parar(velocidade1);
  }
  delay(20);
}

// Função para controlar o carrinho
void controlWalle(char command)
{
  while (command == 'F')
  {
    aceleracao1 += 5;
    if (aceleracao1 > 200)
    {
      aceleracao1 = 200;
    }
    velocidade1 = aceleracao1;
    Frente(velocidade1);
    tipoMovimento = 0;
    Serial.println("Movendo  para frente");
    delay(50);
    if (SerialBT.available())
    {
      command = SerialBT.read(); // Lê o comando enviado pelo aplicativo
    }
  }
  while (command == 'B')
  {
    aceleracao1 += 5;
    if (aceleracao1 > 200)
    {
      aceleracao1 = 200;
    }
    velocidade1 = aceleracao1;
    Tras(velocidade1);
    tipoMovimento = 1;
    Serial.println("Movendo  para trás");
    delay(50);
    if (SerialBT.available())
    {
      command = SerialBT.read(); // Lê o comando enviado pelo aplicativo
    }
  }
  switch (command)
  {
  case 'L': // Esquerda
    Esquerda(velocidade);
    tipoMovimento = 2;
    Serial.println("Virando para esquerda");
    break;

  case 'R': // Direita
    Direita(velocidade);
    tipoMovimento = 3;
    Serial.println("Virando para direita");
    break;

  case 'T': // Parar
    pescoco1();
    break;

  case 'X': // Forward -> próxima música
    AudioTela();
    TocandoAudio();
    break;

  case 'C':
    Olhardireita();
    break;

  case 'S':
    Olharesquerda();
    break;

  case 'G':
    DescerOlho();
    break;

  case 'M':
    SubirOlho();
    break;

  case 'D':
    VirarOlho();
    break;

  case 'W':
    MexerBracoDireito();
    break;

  case 'A':
    MexerBraco();
    break;

  case 'H':
    tempomotor = millis();
    tempomusic = millis();
    TocarMusica();
    Dancarservo();
    break;

  case 'Q':
    cantar();
    break;

  default:
    Serial.println("Comando inválido");
    Parar(velocidade1);
    aceleracao1 = 100;
    break;
  }
}
