# Gotowy projekt A–I: NUCLEO-F767ZI / Mbed OS

Baza: aktualne repozytorium Seb21x/Embedded-systems, commit 78d8bb7.
Zachowano działający układ A–F: main.cpp i osobne klasy w include/ oraz src/.
Nie ma APP_STAGE ani przechwytywania pokrętła. Wszystkie funkcje uruchamiają się razem.

## Podłączenia — najpierw odłącz zasilanie

| Element | Gniazdo / pin | Funkcja |
|---|---|---|
| Temperature Sensor | A0 | Pomiar temperatury |
| Light Sensor | A1 | Pomiar światła |
| Rotary Angle Sensor | A2 | Regulacja progów |
| Zewnętrzny Button | D2 | Puszczony: LOW; trzymany: HIGH |
| Grove LED Socket Kit | D3 | Automatyczne oświetlenie |
| Grove Buzzer v1.2 | D5 | Ostrzeżenia i alarm |
| USER/B1, niebieski przycisk płytki | PC13, bez kabla | Potwierdzenie alarmu |
| Grove RGB LCD | Dowolne gniazdo I2C nakładki | Dane, stan i kalendarz |
| Opcjonalny przyrząd pomiarowy | SIG D7 oraz GND | Pomiar odstępów próbkowania |

Na wcześniej pokazanym zdjęciu gniazda I2C znajdują się w prawej kolumnie
czarnej nakładki. LCD podłącz jednym przewodem Grove do I2C, nie do UART ani D5.
Połączenie I2C zapewnia SDA=D14/PB9, SCL=D15/PB8, VCC i GND.
Nie potrzeba nowych połączeń dla G ani przewodu do USER. D7 nie podłączaj do
kolejnego czujnika: to wyjście testowe, może pozostać niepodłączone.

Zasilanie: napięcie gniazda VCC zależy od nakładki i ustawienia przełącznika.
Sprawdź wymagania konkretnego LCD. Nie przestawiaj całej nakładki na 5 V tylko dla
LCD lub buzzera; wyjścia analogowych czujników nie mogą przekraczać zakresu ADC.
Zachowaj sprawdzone zasilanie A0–A2. W razie osobnego zasilania modułu połącz masy
oraz sprawdź zgodność poziomów sygnału i podciągania I2C. Sam kod nie potwierdza
zgodności elektrycznej. Zdjęcia nie pozwalały odczytać napięcia przełącznika.

D3 i D5 współdzielą TIM1, dlatego oba PWM mają okres 500 us, czyli 2 kHz.
Nie zmieniaj częstotliwości jednego z nich niezależnie od drugiego.

## Uruchomienie

1. Zrób kopię obecnego projektu.
2. Rozpakuj ZIP do NOWEGO folderu. Nie nadpisuj starszego folderu częściowo.
3. Otwórz EMBEDDED-MANDATORY w Mbed Studio. Target: NUCLEO_F767ZI.
4. Pobierz lub zachowaj właściwą zależność mbed-os z mbed-os.lib. Nie jest dołączona
   w całości do ZIP. mbed_app.json zachowuje obsługę printf z liczbami zmiennoprzecinkowymi.
5. Build/Run. Na LCD powinny pojawić się temperatura, stan i przewijane strony danych.
6. Jeżeli IDE zachowało stare wyniki kompilacji, wykonaj Clean i ponownie Build.
7. Terminal jest opcjonalny do diagnostyki; LCD pozwala używać programu bez terminala.

Usunięte pozostałości starej implementacji: EnvironmentMonitor.h/.cpp,
ThresholdModel.h/.cpp i AppConfig.h. Nie kopiuj ich z poprzedniego projektu.
Biblioteka LCD jest już w lib/Grove_LCD_RGB_Backlight. Nie dodawaj drugiej kopii.
Testowa atrapa mbed.h jest tylko w tests/fakes i jest wyłączona z firmware
przez .mbedignore. Nie kopiuj jej do include ani do systemowego mbed-os.

## Co pozostaje jak w A–F

TempSensor, LightSensor, ThresholdController, AlarmController, AlarmButton,
BuzzerController, LightingController i ich interfejsy pozostają bez zmian.
LOW/HIGH: 10–40°C, krok 0,5°C, odstęp >=2°C. D2 wybiera edytowany próg.
Wybrany próg od razu śledzi pokrętło, także po puszczeniu D2. Najwygodniej
ustawić najpierw HIGH z wciśniętym przyciskiem, potem puścić i ustawić LOW.

NORMAL: zielona LED, cisza.
WARNING: niebieska LED, 100 ms dźwięku / 900 ms ciszy.
ALARM: czerwona LED, 150 ms dźwięku / 150 ms ciszy.
ACK: niebieska+czerwona LED, cisza do powrotu do bezpiecznego zakresu.
Błąd temperatury: alarm i TEMP ERROR. USER potwierdza przez InterruptIn na PC13.
LED D3 uzupełnia światło niezależnie od alarmu. Light oznacza procent ADC, nie luksy.

## G — MeasurementFilter i istniejąca histereza

Dwa niezależne obiekty MeasurementFilter filtrują temperaturę i procent światła:

    filtered += 0.2f * (sample - filtered);

Pierwsza próbka inicjalizuje filtr. Przy błędnej temperaturze filtr jest resetowany,
aby po naprawie połączenia nie wracać do starej wartości. Błąd nie zostaje ukryty
przez filtr. Automat dostaje temperaturę filtrowaną, lampa światło filtrowane.

Próbki pobierane są nominalnie co 100 ms. Filtr wprowadza celowe opóźnienie:
po skoku potrzeba około 11 próbek, aby osiągnąć 90% nowej wartości.

Histereza była już w AlarmController: WARNING po wyjściu poza LOW/HIGH;
ALARM przy T<=LOW-3 albo T>=HIGH+3; powrót NORMAL dopiero w zakresie
LOW+0,5 do HIGH-0,5. Wyjście z ALARM do WARNING przy powrocie między
LOW-2,5 i HIGH+2,5, jeśli jeszcze nie spełniono warunku NORMAL.

Test: w terminalu porównaj Traw z T oraz Lraw z Light. Zasłoń czujnik i sprawdź
łagodniejszą zmianę Light. Ustaw próg blisko temperatury i sprawdź stabilność stanów.

## I — DisplayController i Wasza biblioteka LCD

Pierwszy wiersz: temperatura i stan. Drugi wiersz zmienia stronę co 2 sekundy:

1. Procent światła i PWM LED.
2. LOW i HIGH.
3. Wybrany próg LOW/HIGH.
4. Data i godzina w formacie YYYY-MM-DD HH:MM.

Stan pozostaje widoczny na każdej stronie. Kolory podświetlenia: zielony NORMAL,
żółty WARNING, czerwony ALARM, fioletowy ACK. Aktualizacja co około 250 ms;
niezmienione wiersze nie są ponownie wysyłane. Brak clear() w pętli ogranicza migotanie.
Biblioteka korzysta z adresów 7-bit 0x3E (tekst) i 0x62 (RGB), zapisanych dla Mbed
jako 0x7C/0xC4. Jeżeli tekst działa, a kolory nie, sprawdź wersję modułu.
Nowsze kontrolery mogą mieć inne adresy i rejestry; nie zmieniaj tylko adresu w ciemno.
Dostarczona biblioteka nie zwraca błędów I2C — wymagany jest test fizycznego LCD.

### Data kalendarzowa: jawne ograniczenie

CalendarClock jest zegarem programowym. Domyślnie startuje od __DATE__/__TIME__
kompilatora, potem dodaje czas działania programu. Obsługuje północ, zmianę miesiąca,
roku i lata przestępne. Nie pobiera czasu z Internetu ani komputera po USB,
nie używa podtrzymywanego RTC i nie zachowuje upływu czasu po wyłączeniu.
Przy każdym restarcie wraca do daty/godziny zapisanej w firmware.

Przed demonstracją przebuduj main.cpp (najpewniej Clean/Build) i szybko wgraj.
Pozostaje różnica o czas kompilacji/wgrywania i ewentualna różnica strefy kompilatora.
Jeśli chcesz jawnie ustawić datę/godzinę startową, zmień w main.cpp konstruktor np.:

    CalendarClock clock("Nov 20 2026", "12:30:00");

Wpisz własny prawidłowy termin startu, nie przepisuj tego przykładu bezmyślnie.
To realizuje prezentację kalendarza, nie gwarancję aktualnego czasu po dowolnym
restarcie. Gdy prowadzący wymaga takiej gwarancji, potrzebne jest ustawianie RTC
oraz odpowiednie podtrzymanie/synchronizacja — tego nie deklarujemy jako wykonanego.
Dane pomiarowe są wyświetlane niezależnie od kalendarza.

## H — TimingProbe i prawdziwe pomiary

Program wystawia na D7 impuls obejmujący odczyt pokrętła/czujników, obliczenia
filtrów oraz aktualizację lampy. Mierz odstępy zboczy narastających, nie tylko
szerokość impulsu. Docelowy odstęp to około 100 ms; szerokość to czas tego bloku.

Podłączenie przyrządu przy wyłączonym zasilaniu:

- Masa oscyloskopu/analizatora do GND płytki.
- Kanał 1 do SIG D3: PWM LED.
- Kanał 2 do SIG D5: PWM buzzera.
- Dodatkowy kanał lub kolejny pomiar do SIG D7: próbkowanie.
- Opcjonalnie PC13 i D5 jednocześnie: reakcja na USER.

Użyj dostępnych wyprowadzeń tych samych pinów, nie zwieraj VCC z SIG.
Nie odpinaj LED/buzzera, jeśli chcesz mierzyć normalne działanie pod obciążeniem.
Zapisz wyniki w docs/POMIARY_H.csv. Oczekiwane wartości:

| Wielkość | Oczekiwana wartość |
|---|---|
| D3, okres PWM | 500 us / 2 kHz |
| D3 HIGH przy 20/50/80% | 100/250/400 us |
| D5, okres podczas tonu | 500 us |
| D5 HIGH podczas tonu | 250 us |
| WARNING ton/cisza | 100/900 ms |
| ALARM ton/cisza | 150/150 ms |
| D7 odstęp początków impulsów | około 100 ms |
| USER -> wyciszenie | debounce 50 ms plus narzut obsługi |

Dobierz oświetlenie czujnika do badanego PWM, odczytaj zadane LED% z terminala/LCD.
Jeśli masz np. 22%, porównaj pomiar z 110 us, a nie z wartością dla 20%.
Test USER wykonuj podczas tonu, aby zobaczyć krawędź wyłączenia.

Harmonogram współpracujący, sleep_for, I2C i printf wnoszą narzut. To wartości
nominalne, nie gwarancje twardego czasu rzeczywistego. Zapisz rozrzut i warunki;
nie wpisuj ustawień kodu jako wyników pomiarów. Sprawdź z aktywnym LCD i w warunkach
końcowej prezentacji. Bez przyrządu uzgodnij alternatywę z prowadzącym.

## Test końcowy

1. Sprawdź Traw/T i Lraw/Light; ogrzanie czujnika zwiększa temperaturę.
2. Przy T około 24°C ustaw HIGH=28 i LOW=18: NORMAL.
3. HIGH=23: WARNING; HIGH=20 przy LOW<=18: ALARM. Po zmianie HIGH sprawdź LOW,
   ponieważ puszczenie D2 zmienia edytowany próg.
4. USER: ACK i cisza. Przywrócenie zakresu bezpiecznego: NORMAL. Kolejny alarm piszczy.
5. Sprawdź również przekroczenie dolnego progu.
6. Zasłonięcie A1 zwiększa jasność D3. Skieruj lampę z dala od czujnika podczas testu.
7. LCD pokazuje wszystkie strony, kolory i datę. Nastawy da się odczytać bez terminala.
8. Wykonaj i zapisz pomiary H. Użyj normalnego zasilania płytki, nawet gdy odłączysz terminal.

## Walidacja przygotowanego kodu

Testy hostowe: bash tests/run_host_tests.sh (g++).
Sprawdzono automat/histerezę/ACK, progi, PWM, ADC, przycisk, filtr i reset filtra,
przejścia kalendarza, impuls D7 oraz strony/cache LCD. Sprawdzono składnię całej
aplikacji z atrapą Mbed HAL. To nie jest kompilacja ARM/Mbed ani test na sprzęcie.
Punkt H nie jest eksperymentalnie zaliczony, dopóki nie wpiszecie rzeczywistych wyników.
Zmian nie wysłano na GitHub.
