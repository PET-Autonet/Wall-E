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

/*####################################### BLUETOOTH ################################################*/
#define RED_PIN 14
#define GREEN_PIN 27
#define BLUE_PIN 13
const int MAX_MSG_LENGTH = 64;
char receivedMessage[MAX_MSG_LENGTH];
bool newMessage = false;
char command;

#include "RGB.h"
/*#####################################################################################################*/

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

int graucabeca = 100;     // Variável de controle do grau do servo motor da cabeça (servo do canal 4)
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

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  analogWrite(RED_PIN, 128);
  analogWrite(GREEN_PIN, 128);
  analogWrite(BLUE_PIN, 128);

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
    readSerialData();
    if (newMessage)
    {
      Serial.println(receivedMessage);
      if (strlen(receivedMessage) == 2)
      {
        command = receivedMessage[1]; // Pegamos o primeiro caractere como comando
        Serial.print(command);
        controlWalle(command);
      }
      else
      {
        processRGBCommand(receivedMessage);
      }
      newMessage = false;
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
  delay(10);
}

void readSerialData()
{
  static byte index = 1;
  static bool isRGBCommand = false;
  char caracter;
  receivedMessage[0] = 'z';

  while (SerialBT.available() && !newMessage)
  {
    caracter = SerialBT.read();

    // Se for o primeiro caractere e for 'R' seguido de um número, é um comando RGB
    if (index == 1 && caracter == 'R' && SerialBT.peek() >= '0' && SerialBT.peek() <= '9')
    {
      isRGBCommand = true;
    }

    // Se for '/' e estivermos processando um comando RGB
    if (caracter == '/' && isRGBCommand)
    {
      receivedMessage[index] = '\0';
      index = 1;
      newMessage = true;
      isRGBCommand = false;
      return;
    }
    // Se for '/' normal (não RGB)
    else if (caracter == '/' && !isRGBCommand)
    {
      receivedMessage[1] = '/';
      receivedMessage[2] = '\0';
      index = 1;
      newMessage = true;
      return;
    }

    // Se estiver processando um comando RGB, continue acumulando caracteres
    if (isRGBCommand)
    {
      if (index < MAX_MSG_LENGTH - 1)
      {
        receivedMessage[index] = caracter;
        index++;
      }
    }
    // Para comandos não-RGB, processe normalmente
    else if (caracter != receivedMessage[index - 1])
    {
      Serial.print("Caractere recebido: ");
      Serial.println(caracter);

      if (index < MAX_MSG_LENGTH - 1)
      {
        receivedMessage[index] = caracter;
        index++;
        newMessage = true;
      }
    }
  }
}

// Função para controlar o carrinho
void controlWalle(char command)
{
  // Verifica primeiro se é um comando RGB
  if (strlen(receivedMessage) > 2 && receivedMessage[1] == 'R' &&
      isdigit(receivedMessage[2])) // Se começa com 'R' e tem um número depois
  {
    processRGBCommand(receivedMessage);
    return; // Sai da função após processar o comando RGB
  }

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
    delay(50);

    readSerialData();
    if (newMessage)
    {
      if (receivedMessage[1] == '/')
      {
        Serial.println("Parando movimento para frente");
        command = ' ';
        newMessage = false;
        break;
      }
      newMessage = false;
    }
    if (!SerialBT.connected())
    {
      command = ' ';
      break;
    }
  }

  // Movimento para Trás
  while (command == 'B')
  {
    aceleracao1 += 5;
    if (aceleracao1 > 200)
    {
      aceleracao1 = 200;
    }
    velocidade1 = aceleracao1;
    Serial.println(velocidade1);
    Tras(velocidade1);
    tipoMovimento = 1;
    delay(50);

    readSerialData();
    if (newMessage)
    {
      if (receivedMessage[1] == '/')
      {
        Serial.println("Parando movimento para trás");
        Serial.println(velocidade1);
        command = ' ';
        newMessage = false;
        break;
      }
      newMessage = false;
    }
    if (!SerialBT.connected())
    {
      command = ' ';
      break;
    }
  }

  // Cabeça para Direita
  while (command == 'C')
  {
    Olhardireita();
    delay(50);

    readSerialData();
    if (newMessage)
    {
      if (receivedMessage[1] == '/')
      {
        Serial.println("Parando movimento da cabeça");
        command = ' ';
        newMessage = false;
        break;
      }
      newMessage = false;
    }
    if (!SerialBT.connected())
    {
      command = ' ';
      break;
    }
  }

  // Cabeça para Esquerda
  while (command == 'S')
  {
    Olharesquerda();
    delay(50);

    readSerialData();
    if (newMessage)
    {
      if (receivedMessage[1] == '/')
      {
        Serial.println("Parando movimento da cabeça");
        command = ' ';
        newMessage = false;
        break;
      }
      newMessage = false;
    }
    if (!SerialBT.connected())
    {
      command = ' ';
      break;
    }
  }

  switch (command)
  {
  case 'L':
    tipoMovimento = 2;
    Esquerda(velocidade);
    Serial.println("Virando para esquerda");
    break;

  case 'R':
    tipoMovimento = 3;
    Direita(velocidade);
    Serial.println("Virando para direita");
    break;

  case 'T': // Parar
    pescoco1();
    break;

  case 'X': // Forward -> próxima música
    AudioTela();
    TocandoAudio();
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
    Serial.println(velocidade1);
    Parar(velocidade1);
    aceleracao1 = 100;
    break;
  }
}
