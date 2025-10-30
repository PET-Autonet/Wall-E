void processRGBCommand(const char *command)
{
  int r = 0, g = 0, b = 0;
  char *ptr = strchr(command, 'R');

  if (ptr)
  {
    r = atoi(ptr + 1);
    ptr = strchr(ptr, 'G');
    if (ptr)
    {
      g = atoi(ptr + 1);
      ptr = strchr(ptr, 'B');
      if (ptr)
      {
        b = atoi(ptr + 1);

        // Validar valores
        if (r >= 0 && r <= 255 && g >= 0 && g <= 255 && b >= 0 && b <= 255)
        {
          analogWrite(RED_PIN, r);
          analogWrite(GREEN_PIN, g);
          analogWrite(BLUE_PIN, b);
          Serial.printf("LED RGB configurado para R:%d G:%d B:%d\n", r, g, b);
          return;
        }
      }
    }
  }

  Serial.println("Comando RGB inválido. Use o formato: R<valor> G<valor> B<valor>");
}