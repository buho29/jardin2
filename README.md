# jardin2

<h3>sistema de riego en exp32 usando websockets con frontend en vue</h3>

<a href="http://www.youtube.com/watch?feature=player_embedded&v=6sTY9AEmJqI
" target="_blank"><img src="http://img.youtube.com/vi/6sTY9AEmJqI/0.jpg" 
alt="video arduino" width="800" height="600" border="10" /></a>

<a href="http://www.youtube.com/watch?feature=player_embedded&v=kBUrZif1a1I
" target="_blank"><img src="http://img.youtube.com/vi/kBUrZif1a1I/0.jpg" 
alt="video arduino" width="800" height="600" border="10" /></a>

la carpeta app es el proyecto shared

la carpeta jardin 2 contiene el .ino de inicio de la aplicacion y en su carpeta data lo que se debera subir al esp32:
	* data : los datos de la app en formato json
	* www : los ficheros html y demas , lo que se meta aqui es publico

la carpeta example son proyectos probando diferentes partes/clases de la app
la carpeta html :
	* contiene el source html en vue+quasar , si se modifica hay q subirlo en el esp mediante el data o por web

frontend html 
esta en vue + quasar para los componentes , no se usa cli , las libreria js se usa cdn pero local (zipeado en el esp)

el proyect usa estas librerias:

https://github.com/bblanchon/ArduinoJson
https://github.com/me-no-dev/AsyncTCP
https://github.com/me-no-dev/ESPAsyncWebServer
https://github.com/adafruit/Adafruit_BME280_Library
https://github.com/lorol/LITTLEFS
https://github.com/adafruit/RTClib

cuando no usas rtc (real-time clock) hay q comentar el #define __RTC__ en Clock.h
https://github.com/PaulStoffregen/Time