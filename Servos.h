/******************************************************************************************|
|                                                                                          |
|                                                                                          |
|                                                                                          |
|                                FUNÇÃO DOS SERVOS MOTORES                                 |
|                                                                                          |
|                                                                                          |
|                                                                                          |
/******************************************************************************************/
int pulseWidth(int angle){ //Esta função calcula o ângulo de movimento do servo.
int pulse_wide, analog_value;
pulse_wide = map(angle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH); //Esta função pega ângulo de 0 a 180 graus e mapeia do valor mínimo de largura até o valor máximo. 
analog_value = int(float(pulse_wide) / 1000000 * FREQUENCY * 4096);
return analog_value; //O valor que a função retorna.
}
// Movimentos do pescoço
void pescoco1(){
  graupescoco += 1;
  delay(50);
  if(graupescoco == 1){
    for(int i = 100; i < 180; i+=5){
      pwm.setPWM(2, 0, pulseWidth(i));
      delay(20);
    }
    for(int i = 0; i < 80; i+=5){
      pwm.setPWM(3, 0, pulseWidth(i));
      delay(20);
    }
  }
  if(graupescoco == 2){
    for(int i = 80; i < 180; i+=5){
      pwm.setPWM(3, 0, pulseWidth(i));
      delay(20);
    }
  }
  if(graupescoco == 3){
    for(int i = 180; i > 100; i-=5){
      pwm.setPWM(2, 0, pulseWidth(i));
      delay(20);
    }
  }
  if(graupescoco == 4){
    for(int i = 180; i > 0; i-=5){
      pwm.setPWM(3, 0, pulseWidth(i));
      delay(20);
    }
    graupescoco = 0;
  }
  else{
    Serial.println(graupescoco);
    return;
  }
}
// Função para fazer o olho subir e descer
void DescerOlho(){
  olhodireito += 5;
  //olhoesquerdo -= 10;

  Serial.println(olhodireito);
  pwm.setPWM(5, 0, pulseWidth(olhodireito));
  //pwm.setPWM(6, 0, pulseWidth(olhoesquerdo));
}
// Função para subir os olhos
void SubirOlho(){
  olhodireito -= 5;
  //olhoesquerdo += 10;
 
  Serial.println(olhodireito);
  pwm.setPWM(5, 0, pulseWidth(olhodireito));
  //pwm.setPWM(6, 0, pulseWidth(olhoesquerdo));
}
// Função para mover os olhos para um lado
void VirarOlho(){
  if (olhodireito + olhoesquerdo != 180){
    pwm.setPWM(5, 0, pulseWidth(90));
    pwm.setPWM(6, 0, pulseWidth(90));
  }
  olhodireito += 5;
  olhoesquerdo +=5;
  pwm.setPWM(5, 0, pulseWidth(olhodireito));
  pwm.setPWM(6, 0, pulseWidth(olhoesquerdo));
}   

// Função que faz os olhos virar para direita
void Olhardireita(){
  if (graucabeca <= 30){
    return;
  }
  Serial.println(graucabeca);

  graucabeca -= 5;

  pwm.setPWM(4, 0, pulseWidth(graucabeca));
}
// Função que faz os olhos virar para a esquerda
void Olharesquerda(){
  if(graucabeca >= 180){
    return;
  }

  Serial.println(graucabeca);

  graucabeca += 5;

  pwm.setPWM(4, 0, pulseWidth(graucabeca));
}
// Função para mover o braço direito
void MexerBracoDireito(){
  for(int i =0; i < 140; i += 5){
    pwm.setPWM(1, 0, pulseWidth(i));
    delay(20);
    Serial.println(i);
  }
  for(int i =140; i > 0; i -= 5){
    pwm.setPWM(1, 0, pulseWidth(i));
    delay(20);
    Serial.println(i);
  }
}

//Função para mover os dois braços
void MexerBraco(){
  int soma1 = 180;
    for(int j =0; j < 100; j += 5){
    pwm.setPWM(1, 0, pulseWidth(j));
    //if(soma1 > 20){
    pwm.setPWM(0,0,pulseWidth(180- j)); // Braço Esquerdo
    //}
    soma1 = soma1 - j;
    delay(20);
    //Serial.println(i);
  }
  delay(1000);
  int soma = 80;
  for(int i = 100; i > 0; i -= 5){
    pwm.setPWM(1, 0, pulseWidth(i));
    pwm.setPWM(0,0,pulseWidth(soma)); // Braço Esquerdo
    soma = soma + 5 ;
    delay(20);
    Serial.println(i);
    Serial.println(soma);
  }
}
// Função que faz a parte dos servos da dança
void Dancarservo(){
  delay(500);
  while(true){
    for (int i = 0; i <= 30; i++){
      pwm.setPWM(3, 0, pulseWidth(i));
      delay(10);
    }
    for (int i = 30; i >= 0; i--){
      pwm.setPWM(3, 0, pulseWidth(i));
      delay(10);
    }
    if (millis() - tempomusic >= 9000){
      break;
    }
  }
  int soma1 = 180;
  for(int j = 0; j < 100; j += 5){
    pwm.setPWM(1, 0, pulseWidth(j));
    //if(soma1 > 20){
    pwm.setPWM(0,0,pulseWidth(180- j)); // Braço Esquerdo
    //}
    soma1 = soma1 - j;
    delay(20);
  }
  DancarMotor();
  int soma = 80;
  for(int i = 100; i > 0; i -= 5){
    pwm.setPWM(1, 0, pulseWidth(i));
    pwm.setPWM(0,0,pulseWidth(soma)); // Braço Esquerdo
    soma = soma + 5 ;
    delay(20);
  }
  Parar(velocidade1);
}