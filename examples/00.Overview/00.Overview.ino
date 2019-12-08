// MobaLedLib: LED library for model railways
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define EXAMPLE_NR 1

/*
Überblick aller Beispiele:            An English translation is available below.
~~~~~~~~~~~~~~~~~~~~~~~~~~
Dieses Dokument enthält Liste aller Beispielprogramme und erklärt diese stichpunktartig.
Details findet man in den entsprechenden Programmen.


Neu: Seit Version 0.9.0 der Bibliothek gibt es ein Excel Programm mit dem die Funktionen 
     der MobaLedLib ohne zu programmieren genutzt werden können. Man füllt lediglich eine
     Tabelle mit den gewünschten Effekten und schickt diese mit einem Knopfdruck zum Arduino. 
     Das Excel Programm unterstützt den Benutzer mit verschiedenen Dialogen. Damit wird die 
     Erstellung von Licht, Sound und Bewegungseffekten zum Kinderspiel.

     Das Excel Programm wird geöffnet indem man die Tasten Win+R gleichzeitig drückt und die 
     folgende Zeile in den "Start" Dialog kopiert: 
       %USERPROFILE%\Documents\Arduino\libraries\MobaLedLib\extras\Prog_Generator_MobaLedLib.xlsm
     
     Beim ersten Start des Programms wird es in das Verzeichnis 
       %USERPROFILE%\Documents\Arduino\MobaLedLib_ <Lib Version> 
     kopiert damit es verändert und gespeichert werden kann ohne dass die Bibliothek
     beeinflusst wird. 
     Es wird ein Link auf dem Windows Desktop angelegt über den das Programm danach 
     gestartet wird:
         Prog_Generator
         MobaLedLib



Dieses Beispiel wurde eingefügt, weil es manchmal schwierig zu verstehen ist was sich
unter den kurzen Namen im Menü der Arduino IDE verbirgt.
Es bietet außerdem die Möglichkeit die Programme ganz schnell zu testen ohne dass man
jedes einzelne Programm laden muss. Dazu trägt man einfach die Nummer des gewünschten
Beispiels in die "#define EXAMPLE_NR" Zeile ganz oben ein und startet das Programm mit
STRG+U.

Achtung einige Beispiele benötigen neben mehreren RGB LEDs noch zusätzliche Komponenten.
Dies wird jeweils in Klammern unten angegeben.

01.Heartbeat:     (Eine LED)
~~~~~~~~~~~~~
Dieses Beispiel ist das "Hello World" Programm der WS281x LEDs. Es zeigt Eindrucksvoll
wie einfach man mit der Bibliothek eine LED mit wechselnden Farben sanft auf und
abblenden kann. Dieses Blinken kann auch in einem eignen Programm zur Funktionskontrolle
eingesetzt werden.

02.House:         (7 LEDs)
~~~~~~~~~
In einem "belebten" Haus sind nicht immer alle Zimmer beleuchtet. Je nachdem in welchen
Räumen sich die Benutzer aufhalten wird das Licht ein- und ausgeschaltet. Außerdem kann
die Art der Beleuchtung ganz unterschiedlich sein. Mit der MobaLedLib können verschiedene
Varianten simuliert werden. Räume mit warmem oder kalten Licht, Räume welche von Neonröhren
beleuchtet werden und Zimmer in denen ein Fernseher läuft und vieles mehr. Das Beispiel
zeigt einen Teil davon.

03.Switched_Houses:   (16 LEDs, opt. 4 Kippschalter)
~~~~~~~~~~~~~~~~~~~
Mit diesem Beispiel wird gezeigt wie man mehrere Häuser über Kippschalter Ein- und
Ausschaltet. Es zeigt, dass langsam nacheinander die verschiedenen Lichter aktiviert
werden. Eigentlich benötigt man zum Test 4 Kippschalter. Wenn keine Schalter
angeschlossen sind, dann werden die Lichter in allen drei Häusern zufällig eingeschaltet.

04.Gaslights:       (6 LEDs)
~~~~~~~~~~~~~
Gas betriebene Straßenbeleuchtungen gehörten früher zum Straßenbild.  Sie wurden nicht
alle gleichzeitig angezündet, sondern nacheinander. Ursprünglich war das die Aufgabe
eines "Laternenanzünders". Später wurden diese durch Zünduhren aktiviert. Auch heute noch
werden Gaslampen nacheinander per Lichtsensor gestartet (http://www.gaswerk-augsburg.de/fernzuendung.html).
Dieses Beispiel zeigt wie das auf der Modelleisenbahn nachempfunden werden kann.
Dabei werden auch das langsam heller werdende Licht und das Flackern mancher Lampen
nachgebildet.

05.St.AndrewsCross:   (2 LEDs)
~~~~~~~~~~~~~~~~~~~
Auf einer Modelleisenbahn gibt es natürlich auch Bahnübergänge. Diese werden über 
Wechselblinker gesichert. Dabei werden die Lichter langsam heller und dunkler wie in der 
Realität was einen schönen Effekt ergibt.

06.Sound:       (MP3-TF-16P oder JQ6500 Sound Modul & Taster)
~~~~~~~~~
Für dieses Beispiel wird entweder ein MP3-TF-16P Sound Modul oder ein JQ6500 Modul 
benötigt. Außerdem werden noch einige weitere Bauteile eingesetzt. Im Programm findet 
man die benötigten Schaltpläne als ASCII Grafik und im „extras“ Verzeichnis der 
Bibliothek als PDF Datei. Anstelle eines Sound Moduls kann auch eine RGB LED 
angeschlossen werden. Diese Blitzt kurz auf, wenn die Befehle an das Sound Modul 
geschickt werden. 

07.Macro_Fire_truck:    (5 LEDs)
~~~~~~~~~~~~~~~~~~~~
Die Feuerwehr darf natürlich auch im Model nicht fehlen. Dieses Beispiel zeigt wie man 
das Blitzen des Blaulichts und das Blinken von Frontscheinwerfern und Warnblinkern 
nachbilden kann. Das Beispiel kann entweder mit RGB LEDs oder über WS2811 Module auch mit
kleinen im Fahrzeug eingebauten LEDs betrieben werden. Letzteres sieht natürlich viel 
realistischer aus. Da man normalerweise kein entsprechendes Fahrzeug zur Hand hat, wenn 
man das Beispiel testet sind Standardmäßig die RGB LEDs aktiviert.

08.Constr.Warn.Light_Pattern_Func:  (9 LEDs)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Zur Absicherung von Baustellen werden oft Lauflichter eingesetzt. Solch einen Effekt kann
man wunderschön mit der MobaLedLib erzeugen. Die Leuchtmuster sind dabei beliebig 
konfigurierbar. In dem Beispiel wird kurz erklärt wie das gemacht wird. Es lohnt sich 
einen Blick in das Programm zu werfen.

09.TrafficLight_Pattern_Func:   (6 LEDs)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Die Muster einer Verkehrsampel können ebenfalls sehr einfach über die Bibliothek 
konfiguriert werden. Das Beispiel zeigt eine einfache Kreuzung mit zwei Ampeln. Es ist 
aber auch möglich viel kompliziertere Ampeln mit mehreren Spuren und Fußgängerampeln zu 
erstellen. Im Programm wird beschrieben wie das geht.

10.RailwaySignal_Pattern_Func:  (3/5 LEDs, 1 oder 2 Taster)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Mit diesem Beispiel wird gezeigt wie man Lichtsignale mit der MobaLedLib ansteuert. Es 
benötigt neben den LEDs noch einen oder zwei Taster mit denen die Signalbilder gewechselt 
werden. Standardmäßig wird ein Einfahrtsignal mit den drei Aspekten Hp0, Hp1 und Hp2 
dargestellt. Durch ein/auskommentieren zweier Zeilen kann auch ein Ausfahrtsignal 
generiert werden. Im Beispiel wird erklärt wie solche Signale konfiguriert werden so dass
man in der Lage ist beliebige Signalbilder zu erstellen.
Der Wechsel von einem Bild zum nächsten geschieht fließend. Dazu wird die Helligkeit der 
LEDs langsam auf- und abgeblendet. 

11.CAN_Bus_MS2_RailwaySignal:   (3/5 LEDs, CAN Modul)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Dieses Beispiel entspricht dem vorangegangenen. Allerdings werden hier die Signalbilder 
mit einer Märklin MS2 über den CAN Bus gesteuert. Zum Testen wird ein MCP2515 CAN Modul 
für 1.50€ benötigt. Im Programm findet man die dazu benötigten Verbindungen.

12.CAN_Bus_MS2_Switch_Lights:   (16 LEDs, CAN Modul)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Mit diesem Beispiel wird gezeigt wie man die Lichter verschiedener Häuser mit einer 
Märklin MS2 über den CAN Bus gesteuert. Das Beispiel entspricht bis auf die Steuerung dem
Beispiel 03. Zum Testen benötigt man ebenso wie im letzten Beispiel ein CAN Modul.

13.FlipFlop_Counter:      (32 LEDs, 2 Taster)
~~~~~~~~~~~~~~~~~~~~
Mit Flip-Flops und Zählern lassen sich die verschiedensten Effekte realisieren. Mit 
diesem Beispiel werden ein Paar Funktionen demonstriert. Das Beispiel wird über zwei 
Taster gesteuert. 

14.Switches_80_and_more:    (viele LEDs, Taster und CD4017)
~~~~~~~~~~~~~~~~~~~~~~~~
Das Beispiel zeigt wie man mit einem Arduino über nur 10 Prozessor Pins und einem
zusätzlichen IC für 31 Cent (CD4017) 80 Schalter einlesen kann. Mit zwei weiteren CD4017
können 208 Schalter gelesen werden. Die Bibliothek unterstützt zwei Gruppen von Schaltern. 
Eine kann z.B. in einem Weichenstellpult untergebracht sein. Die zweite Gruppe ist für 
"Druck-Knopf" Aktionen am Anlagenrand gedacht. 
Bei der Methode werden beliebig viele gleichzeitig aktive Schalter erkannt. Die Anzahl 
und welche Pins benutzt werden ist frei konfigurierbar. 
In dem Beispiel Programm werden 146 LEDs über die Schalter gesteuert. Das Beispiel zeigt 
auch mit welcher unglaublichen Geschwindigkeit die Bibliothek mit dieser großen Menge an 
Ein- und Ausgabeoperationen zurechtkommt.

15.Sound_and_Keyboard:      (Sound Modul, Taster und CD4017)
~~~~~~~~~~~~~~~~~~~~~~
Mit diesem Programm wird gezeigt wie man die verschiedenen Funktionen eines Sound Moduls 
nutzt. Es kann mit einem MP3-TF-16P oder einem JQ6500 Sound Modul benutzt werden. Über 
die einzelnen Taster können gezielt einzelne Sounds abgerufen werden. Beim MP3-TF-16P 
sind das 14 verschiedene Geräusche (5 beim JQ6500). Zusätzlich können einige weiter 
Funktionen per Tastendruck abgerufen werden.

16.Illumination_Pattern_Func:   (13 LEDs, Taster)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"Knopf-Druck" Aktionen sind eine Attraktion für jede Anlage. Mit der MobaLedLib kann man 
das einfach umsetzen. Dieses Beispiel stammt aus unserer Anlage. Per Knopfdruck kann man 
die Beleuchtung unserer Höhle aktivieren. Beim ersten Druck auf den Taster wird eins der 
beiden Lagerfeuer "angezündet". Mit dem zweiten Tastendruck illuminieren verschiedene 
bunte Strahler die Höhle. Dabei wechseln die Farben langsam. Mit dem dritten Druck wird 
ein ähnlicher Effekt aktiviert bei dem allerdings mehr LEDs gleichzeitig aktiv sind. 

17.Darkness_Detection:      (28 LEDs, LDR, Schalter)
~~~~~~~~~~~~~~~~~~~~~~
Das automatische Ein- und Ausschalten der Beleuchtung auf der Modelleisenbahn kann über 
einen LDR (Licht abhängiger Widerstand) gelöst werden. Wenn die Beleuchtung im Moba 
Zimmer langsam herunter gedimmt wird werden automatisch die verschiedenen Lichter auf der
Anlage eingeschaltet. Entsprechend werden sie wieder abgeschaltet, wenn es "Tag" wird. 
Auf diese Weise kann man sehr einfach eine automatische Lichtsteuerung umsetzen. Mit 
einem Schalter kann auch Manuel auf Tag oder Nacht geschaltet werden. Dann werden die 
Effekte Zeitgesteuert aktiviert. 

18.Schedule:        (28 LEDs)
~~~~~~~~~~~~
Das Schedule Beispiel entspricht dem vorangegangenen Beispiel. Allerdings wird hier das 
Ein- und Ausschalten der Beleuchtung über eine Modellbahnzeit gesteuert. Damit lassen 
sich viel realistischerer Abläufe erzeugen. Für das Beispiel wird eine interne 
Modellbahnzeit benutzt. Diese kann natürlich auch durch eine globale Zeit eine 
Steuerungssoftware ersetzt werden. Dann können Beleuchtungen und Umgebungslicht gemeinsam
beeinflusst werden. 

19.Logic:         (8 LEDs, 4 Taster, 1 Kippschalter)
~~~~~~~~~
Mit der MobaLedLib können auch beliebige logische Verknüpfungen implementiert werden. In 
diesem Beispiel wird gezeigt wie man die Ansteuerung eines Andreaskreuzes in einem 
Bereich einer Weiche in Abhängigkeit der Weichenstellung aktiviert.

20.Huge_Configuration:      (256 LEDs, CAN Bus)
~~~~~~~~~~~~~~~~~~~~~~
Dieses Beispiel zeigt, das man tatsächlich 256 RGB LEDs (= 786 einzelne LEDs) mit einem 
Arduino ansteuern kann. Dabei können die einzelnen LEDs immer noch so schnell geschaltet
werden, dass Effekte wie das doppelte Blitzen eines Blaulichts möglich sind. Die Update 
Zeit bei dieser Riesigen Konfiguration liegt bei 14ms! Das ist schneller als das 
Menschliche Auge wahrnehmen kann. 
Außerdem zeigt das Beispiel, dass die Bibliothek so sparsam ist, dass nur 63% des FLASH
Speichers benötigt werden. 
Für Tests ist es nicht nötig das tatsächlich 256 LEDs angeschlossen sind. Es spielt für
die Performance keine Rolle ob die generierten Signale an einer LED ankommen oder nicht. 
Wenn alle LEDs eingeschaltet sind würde ein Strom von 14A fließen. Darum nutzt das 
Beispiel die "setMaxPowerInVoltsAndMilliamps()" Funktion zur Begrenzung des Maximalen 
Stromes.

21.Advanced:        (…)
~~~~~~~~~~~~
Dieses Beispiel zeigt, dass mit der Bibliothek fast alles möglich ist. Details findet man 
im Programm.

22.Burning_House:       (16 LEDs, Taster, ...)
~~~~~~~~~~~~~~~~~
Hier wird gezeigt wie eine komplexe "Druck Knopf" konfiguriert werden kann. Durch 
mehrfaches drücken auf einen Taster können verschiedene Funktionen aktiviert werden
(LEDs, Rauchgenerator und Sound). 
Mit dem ersten Druck auf den Taster wird das Feuer "entzündet". Der zweite Knopfdruck
aktiviert den Rauchgenerator. Und mit dem dritten Druck auf den Taster kommt die 
Feuerwehr mit Blaulicht und Martinshorn. Weil Kinder gerade an den Geräuschen Spaß haben
kann man mit jedem weiteren Tastendruck eine andere MP3 Datei abrufen. Das Video im Post
#58: https://www.stummiforum.de/viewtopic.php?p=1924210#p1924210 zeigt das sehr schön. 


25.Analog_Push_Button:      (32 LEDs, Taster, Widerstände)
~~~~~~~~~~~~~~~~~~~~~~
In den vorangegangenen Beispielen wurde zum einlesen mehrerer Taster das zusätzliches 
Zähler IC (CD4017) benutzt. Es geht aber auch einfacher. An einem analogen Eingang eines
Arduinos kann man bis zu 10 Taster anschließen welche einfach über verschiedene 
Widerstände kodiert werden. Für bis zu 10 „Druck Knopf“ Aktionen recht damit ein 
zweipoliges Kabel welches von einem zum nächsten Schalter läuft. Der Nachteil dieser sehr
einfachen Schaltung ist, dass es zu falschen Signalen kommt, wenn mehrere Taster 
gleichzeitig betätigt werden. 
Das Beispiel zeigt 5 verschiedene „Druck Knopf“ Aktionen (Gaslaternen, Belebtes Haus, 
Feuerwehr Auto, Sound und Burg Beleuchtung)


*******************
* English version *
******************* 


Overview of all examples:
~~~~~~~~~~~~~~~~~~~~~~~~~~
This document contains a list of all sample programs and explains them in a nutshell. 
Details can be found in the corresponding programs. 


New: Since version 0.9.0 of the library there is an Excel program with which the functions 
     of the MobaLedLib can be used without programming. You simply fill a table with the 
     desired effects and send them to the Arduino with the push of a button. The Excel 
     program supports the user with various dialogs. This makes the creation of light, 
     sound and motion effects a breeze.

     To start the excel program press Win+r key together and copy the following line 
     into the "Run" dialog and press enter:
       %USERPROFILE%\Documents\Arduino\libraries\MobaLedLib\extras\Prog_Generator_MobaLedLib.xlsm

     The first time you start the program it will be copied in the directory 
       %USERPROFILE%\Documents\Arduino\MobaLedLib_ <Lib Version> 
     so it can be changed and saved without the library being affected.
     A link will be created on the Windows desktop via the program afterwards
     is started:
         Prog_Generator
         MobaLedLib




This example has been inserted because it is sometimes difficult to understand what a 
short name means. It also offers the possibility to test the programs very fast without 
having to load every single program. Just enter the number of the desired example in the 
"#define EXAMPLE_NR" line at the top and start the program with CTRL + U.
Please note that some examples require additional components in addition to several RGB 
LEDs. This is indicated in brackets below.

01.Heartbeat:           (One LED)
~~~~~~~~~~~~~
This example is the "Hello World" program of the WS281x LEDs. It shows impressively how 
easy it is to light up and fade a LED with changing colors gently with the library. This
flashing can also be used in the program to check the “health” of the program.

02.House: (7 LEDs)
~~~~~~~~~
In a "busy" house, not all rooms are lit. Depending on the rooms in which the residents 
are staying, the light is switched on and off. In addition, the type of lighting can be 
quite different. With the MobaLedLib different variants can be simulated. Rooms with 
warm or cold light, rooms which are lit by neon lights and rooms where a TV is running 
and much more. The example shows a part of it.

03.Switched_Houses:     (16 LEDs, opt 4 toggle switch)
~~~~~~~~~~~~~~~~~~~
This example shows how to switch several houses on and off via toggle switches. It shows 
that the different lights are activated slowly in succession. Actually, you need to test 
4 toggle switches. If no switches are connected, the lights in all three houses will be 
switched on at random.

04.Gaslights:           (6 LEDs)
~~~~~~~~~~~~~
Gas-powered street lighting used to be part of the streetscape. They were not lit all at 
once, but one after the other. Originally this was the task of a "lamplighter". 
Later, these were activated by "Ignition clocks". Even today, gas lamps are started 
asynchronous by light sensors (http://www.gaswerk-augsburg.de/fernzuendung.html). This 
example shows how this can be modeled on the model railway. The example shows how the 
slowly brightening light and the flickering of some lamps are reproduced.

05.St.AndrewsCross:     (2 LEDs)
~~~~~~~~~~~~~~~~~~~
On a model railway, of course, there are also railroad crossings. These are secured by 
change indicators. The lights are getting brighter and darker as in reality which gives 
a nice effect.

06.Sound:               (MP3-TF-16P or JQ6500 sound module & button)
~~~~~~~~~
For this example, either an MP3-TF-16P sound module or a JQ6500 module is needed. In 
addition, some more components are used. The program contains the required circuit 
diagrams as ASCII graphics. A corresponding PDF file could be found in the “extras” 
directory of the library. Instead of a sound module, an RGB LED can also be connected. 
This flashes briefly when the commands are sent to the sound module.

07.Macro_Fire_truck:    (5 LEDs)
~~~~~~~~~~~~~~~~~~~~
Of course, the fire department should not be missing in the model. This example shows 
how to reproduce the flashing of the blue light and the flashing of headlights and hazard 
lights. The example can be operated either with RGB LEDs or via WS2811 modules also with 
small LEDs installed in the vehicle. The latter of course looks much more realistic. 
Since you usually do not have a suitable vehicle at hand when you test the example, the 
RGB LEDs are activated by default.

08.Constr.Warn.Light_Pattern_Func: (9 LEDs)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
To protect construction sites, running lights are often used. Such an effect can be 
created beautifully with the MobaLedLib. The light patterns are arbitrarily configurable. 
The example briefly explains how this is done. It is worth to take a look at the program.

09.TrafficLight_Pattern_Func: (6 LEDs)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The patterns of a traffic light can also be easily configured via the library. The 
example shows a simple intersection with two traffic lights. But it is also possible to 
create much more complicated traffic lights with several tracks and pedestrian lights. 
The program describes how it works.

10.RailwaySignal_Pattern_Func: (3/5 LEDs, 1 or 2 buttons)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This example shows how to control light signals with the MobaLedLib. In addition to the 
LEDs one or two buttons are required to change the signal images. By default, an entry 
signal is displayed with the three aspects Hp0, Hp1 and Hp2. By commenting / commenting 
out two lines, an exit signal can also be generated. The example explains how such 
signals are configured so that one is able to create any signal images.
The change from one picture to the next happens fluently. For this purpose, the 
brightness of the LEDs is slowly faded in and out.

11.CAN_Bus_MS2_RailwaySignal: (3/5 LEDs, CAN module)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This example corresponds to the previous one. However, here the signal images are 
controlled with a Märklin MS2 via the CAN bus. For testing an MCP2515 CAN module is 
needed for 1.50 €. In the program you will find the necessary connections.

12.CAN_Bus_MS2_Switch_Lights: (16 LEDs, CAN module)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This example shows how to control the lights of different houses with a Märklin MS2 via 
the CAN bus. Except for the controller, the example corresponds to Example 03. For 
testing, just as in the last example, a CAN module is required.

13.FlipFlop_Counter:    (32 LEDs, 2 buttons)
~~~~~~~~~~~~~~~~~~~~
With flip-flops and counters a variety of effects can be realized. This example 
demonstrates a couple of functions. The example is controlled by two buttons.

14.Switches_80_and_more: (many LEDs, buttons and CD4017)
~~~~~~~~~~~~~~~~~~~~~~~~
The example shows how to read 80 switches with an Arduino using only 10 processor pins 
and an additional IC for 31 cents (CD4017). With two more CD4017 up to 208 switches can 
be read. The library supports two groups of switches. One can e.g. be housed in a points 
console. The second group is intended for "push button" actions on the edge of the model 
railway.
The method recognizes any number of simultaneously active switches. The number and which 
pins are used is freely configurable.
In the example program, 146 LEDs are controlled via the switches. The example also shows 
the unbelievable speed with which the library copes with this large amount of input and 
output operations.

15.Sound_and_Keyboard:   Sound module, buttons and CD4017)
~~~~~~~~~~~~~~~~~~~~~~
This program shows how to use the different functions of a sound module. It can be used 
with an MP3-TF-16P or a JQ6500 sound module. Individual buttons can be used to 
selectively recall individual sounds. The MP3 TF-16P has 14 different sounds (5 for the 
JQ6500). In addition, some additional functions can be called up by pressing a button.

16.Illumination_Pattern_Func: (13 LEDs, buttons)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"Push button" actions are an attraction for any model railway. With the MobaLedLib you 
can easily implement this. This example comes from our railway. By pressing a button, 
you can activate the lighting of our cave. When you press the button for the first time, 
one of the two campfires are "lit". With the second push of a button different colorful 
spotlights illuminate the cave. The colors change slowly. The third pressure activates a 
similar effect but with more LEDs active at the same time.

17.Darkness_Detection: (28 LEDs, LDR, Switch)
~~~~~~~~~~~~~~~~~~~~~~
The automatic switching of the lighting on the model railway can be solved via an LDR 
(light dependent resistance). When the lighting in the Moba room is slowly dimmed down, 
the various lights on the system are automatically switched on. Accordingly, they are 
turned off again when it becomes "day". In this way, you can easily implement an 
automatic lighting control. A toggle switch could also be used to select day or night. 
Then the effects are activated time-controlled.

18.Schedule: (28 LEDs)
~~~~~~~~~~~~
The schedule example corresponds to the previous example. However, here the switching on 
and off of the lighting is controlled over a model railway time. This allows much more 
realistic processes to be generated. For the example, an internal model train time is 
used. This can of course also be replaced by a global time from a control software. Then
lighting and ambient light can be influenced together.

19.Logic: (8 LEDs, 4 buttons, 1 toggle switch)
~~~~~~~~~
The MobaLedLib can also be used to implement arbitrary logical links. This example shows 
how to activate the control of a St. Andrew's cross in an area of a turnout depending on 
the turnout position.

20.Huge_Configuration: (256 LEDs, CAN bus)
~~~~~~~~~~~~~~~~~~~~~~
This example shows that you can actually drive 256 RGB LEDs (= 786 individual LEDs) with 
an Arduino. The individual LEDs can still be switched so fast that effects such as the 
double flash of a blue light are possible. The update time for this huge configuration is 
only 14ms! This is faster than the human eye can perceive.
In addition, the example shows that the library is so economical that only 63% of the 
FLASH memory is needed.
For tests, it is not necessary that actually 256 LEDs are connected. It does not matter 
for the performance whether the generated signals arrive at an LED or not.
If all LEDs are on, a current of 14A would flow. Therefore, the example uses the 
"setMaxPowerInVoltsAndMilliamps()" function to limit the maximum current.

21.Advanced:          (...)
~~~~~~~~~~~~
This example shows that almost everything is possible with the library. Details can be 
found in the program.

22.Burning_House:     (16 LEDs, buttons, ...)
~~~~~~~~~~~~~~~~~
Here is shown how a complex "pressure button" can be configured. Press several times on 
a button to activate various functions (LEDs, smoke generator and sound).
The first pressure on the button ignites the fire. The second push of the button 
activates the smoke generator. And with the third push on the button the fire truck is 
activated with lights and sound. Because children just enjoy the sounds, you can call up 
another MP3 file each time you press the button. The video in post #58: 
https://www.stummiforum.de/viewtopic.php?p=1924210#p1924210 shows that very nice.


25.Analog_Push_Button: (32 LEDs, buttons, resistors)
~~~~~~~~~~~~~~~~~~~~~~
In the previous examples, the additional counter IC (CD4017) was used to read several 
buttons. This example shows that it could be implemented much easier. One analogue input 
of an Arduino could be connected up to 10 pushbuttons which are simply coded by different
resistors. For those 10 "push button" actions a two-pole cable running from one switch to
the next is required. The disadvantage of this very simple circuit is that false signals 
occur when several buttons are pressed simultaneously. 
The example shows 5 different "Pressure Button" actions (Gas Lanterns, Animated House, 
Fire Department Auto, Sound and Castle Lighting)

--------------------------------------------------------------------------------------------------------------
 Copyright (C) 2018, 2019  Hardi Stengelin: MobaLedLib@gmx.de

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 -------------------------------------------------------------------------------------------------------------
*/


#if   EXAMPLE_NR == 1
   #include "01.Heartbeat\01.Heartbeat.h"
#elif EXAMPLE_NR == 2
   #include "02.House\02.House.h"
#elif EXAMPLE_NR == 3
   #include "03.Switched_Houses\03.Switched_Houses.h"
#elif EXAMPLE_NR == 4
   #include "04.Gaslights\04.Gaslights.h"
#elif EXAMPLE_NR == 5
   #include "05.St.AndrewsCross\05.St.AndrewsCross.h"
#elif EXAMPLE_NR == 6
   #include "06.Sound\06.Sound.h"
#elif EXAMPLE_NR == 7
   #include "07.Macro_Fire_truck\07.Macro_Fire_truck.h"
#elif EXAMPLE_NR == 8
   #include "08.Constr.Warn.Light_Pattern_Func\08.Constr.Warn.Light_Pattern_Func.h"
#elif EXAMPLE_NR == 9
   #include "09.TrafficLight_Pattern_Func\09.TrafficLight_Pattern_Func.h"
#elif EXAMPLE_NR == 10
   #include "10.RailwaySignal_Pattern_Func\10.RailwaySignal_Pattern_Func.h"
#elif EXAMPLE_NR == 11
   #include "11.CAN_Bus_MS2_RailwaySignal\11.CAN_Bus_MS2_RailwaySignal.h"
#elif EXAMPLE_NR == 12
   #include "12.CAN_Bus_MS2_Switch_Lights\12.CAN_Bus_MS2_Switch_Lights.h"
#elif EXAMPLE_NR == 13
   #include "13.FlipFlop_Counter\13.FlipFlop_Counter.h"
#elif EXAMPLE_NR == 14
   #include "14.Switches_80_and_more\14.Switches_80_and_more.h"
#elif EXAMPLE_NR == 15
   #include "15.Sound_and_Keyboard\15.Sound_and_Keyboard.h"
#elif EXAMPLE_NR == 16
   #include "16.Illumination_Pattern_Func\16.Illumination_Pattern_Func.h"
#elif EXAMPLE_NR == 17
   #include "17.Darkness_Detection\17.Darkness_Detection.h"
#elif EXAMPLE_NR == 18
   #include "18.Schedule\18.Schedule.h"
#elif EXAMPLE_NR == 19
   #include "19.Logic\19.Logic.h"
#elif EXAMPLE_NR == 20
   #include "20.Huge_Configuration\20.Huge_Configuration.h"
#elif EXAMPLE_NR == 21
   #include "21.Advanced\21.Advanced.h"
#elif EXAMPLE_NR == 22
   #include "22.Burning_House\22.Burning_House.h"
#elif EXAMPLE_NR == 25
   #include "25.Analog_Push_Button\25.Analog_Push_Button.h"
 #else
  #error EXAMPLE_NR is invalid
#endif
