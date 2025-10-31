/******************************************************************************************|
|                                                                                          |
|                                                                                          |
|                                                                                          |
|                                FUNÇÃO DOS MOTORES                                        |
|                                                                                          |
|                                                                                          |
|                                                                                          |
/******************************************************************************************/

// Função para fazer o Wall-E ir para frente
void Frente(int intensidade)
{
  Serial.println(intensidade);
  analogWrite(IN[0], intensidade);
  analogWrite(IN[1], LOW);
  analogWrite(IN[2], intensidade);
  analogWrite(IN[3], LOW);
}

// Função para fazer o Wall-E ir para trás
void Tras(int intensidade)
{
  analogWrite(IN[0], LOW);
  analogWrite(IN[1], intensidade);
  analogWrite(IN[2], LOW);
  analogWrite(IN[3], intensidade);
}

// Função para fazer o Wall-E ir para direita
void Esquerda(int intensidade)
{
  analogWrite(IN[0], LOW);
  analogWrite(IN[1], intensidade);
  analogWrite(IN[2], intensidade);
  analogWrite(IN[3], LOW);
}

// Função para fazer o Wall-E ir para esquerda
void Direita(int intensidade)
{
  analogWrite(IN[0], intensidade);
  analogWrite(IN[1], LOW);
  analogWrite(IN[2], LOW);
  analogWrite(IN[3], intensidade);
}

// Função para parar o Wall-E
void Parar(int intensidade)
{
  if (tipoMovimento == 0)
  {
    for (int j = intensidade; j >= 0; j -= 5)
    {
      for (int i = 0; i < 4; i++)
      {
        if (i % 2 == 1)
        {
          analogWrite(IN[i], LOW);
        }
        else
        {
          analogWrite(IN[i], j);
        }
      }
      delay(30);
    }
    velocidade1 = 0;
  }

  if (tipoMovimento == 1)
  {
    for (int j = intensidade; j >= 0; j -= 5)
    {
      for (int i = 0; i < 4; i++)
      {
        if (i % 2 == 1)
        {
          analogWrite(IN[i], j);
        }
        else
        {
          analogWrite(IN[i], LOW);
        }
      }
      delay(30);
    }
    velocidade1 = 0;
  }

  if (tipoMovimento == 2)
  {
    for (int i = 0; i < 4; i++)
    {
      analogWrite(IN[i], LOW);
    }
  }

  if (tipoMovimento == 3)
  {
    for (int i = 0; i < 4; i++)
    {
      analogWrite(IN[i], LOW);
    }
  }
}
// Função que faz a parte dos motores da dança
void DancarMotor()
{
  tempomotor = millis();
  while (true)
  {
    Direita(velocidade);
    if (millis() - tempomotor >= 5000)
    {
      break;
    }
  }
  tempomotor = millis();
  Parar(velocidade1);
  delay(20);
  while (true)
  {
    Esquerda(velocidade);
    if (millis() - tempomotor >= 5000)
    {
      break;
    }
  }
}
