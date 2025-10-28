/******************************************************************************************|
|                                                                                          |
|                                                                                          |
|                                                                                          |
|                                   FUNÇÃO DO MP3                                          |
|                                                                                          |
|                                                                                          |
|                                                                                          |
/******************************************************************************************/
// Função que toca música e passa para proxima
void TocandoAudio(){
  currentTrack++;
  if (currentTrack > sizeaudio) { // Limita a 3 faixas
    currentTrack = 1; // Volta para a primeira faixa
  }
  Serial.print("Tocando faixa ");
  Serial.println(currentTrack);
  myMP3.play(currentTrack);
  isPlaying = true;
}
void cantar(){
  myMP3.play(6);
  isPlaying = true;
}
void TocarMusica(){
  myMP3.play(4);
  isPlaying = true;
}