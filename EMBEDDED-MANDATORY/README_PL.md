# Monitor środowiska — instrukcja B–I

Kod przygotowany na podstawie repozytorium Seb21x/Embedded-systems, commit
5044c333e318d36ebe33b3886af49cdd0b17e7c8, oraz załączonego zadania AME26_27.
Język: C++ / Mbed OS. Cel: NUCLEO_F767ZI. Źródła LCD pochodzą z przesłanego
Grove_LCD_RGB_Backlight.zip, z małymi poprawkami opisanymi poniżej.

**Status:** implementacja przygotowana i sprawdzona testami na komputerze.
Nie wykonano kompilacji narzędziami ARM/Mbed ani testów na rzeczywistej płytce.
Punkt H pozostaje do wykonania na stanowisku. Wyników pomiarów nie wymyślono.

## 1. Wgranie projektu

1. Zachowaj kopię obecnego projektu.
2. Rozpakuj paczkę. Otwórz katalog EMBEDDED-MANDATORY jako projekt Mbed w Mbed Studio.
   Jeżeli pracujesz w istniejącym projekcie, zastąp main.cpp, katalogi include i src,
   dodaj lib i .mbedignore. Nie zostawiaj drugiego main.cpp ani starych kopii .cpp
   wewnątrz projektu — kompilator może je również znaleźć.
3. Zachowaj/pobierz Mbed OS wskazany przez mbed-os.lib. Paczka nie zawiera całego OS.
   Nie zmieniaj wersji Mbed OS tylko po to, żeby dodać te klasy.
4. Ustaw target NUCLEO_F767ZI. Uruchom Build, a następnie Run/flash zgodnie z Waszą
   dotychczas działającą konfiguracją płytki. W razie błędu potrzebny jest pierwszy
   pełny komunikat kompilatora, nie tylko podsumowanie liczby błędów.
5. Otwórz terminal szeregowy z prędkością używaną przez Wasz projekt.
6. W include/AppConfig.h początkowo jest APP_STAGE 2. Ten tryb uruchamia A i B,
   bez buzzera i bez inicjalizacji LCD. Po zmianie APP_STAGE wykonaj ponowny Build/Run.
7. .mbedignore wyłącza tests/ i docs/ z firmware. Tests zawiera atrapę mbed.h
   wyłącznie do testów na komputerze; nigdy nie kopiuj jej do include ani do Mbed OS.

| APP_STAGE | Aktywne funkcje |
|---|---|
| 2 | A, nastawy B, terminal, diody stanu na płytce |
| 3 | Dodatkowo automat C, alarm logiczny B i filtr/histereza G |
| 4 | Dodatkowo potwierdzenie D przez USER/PC13 |
| 5 | Dodatkowo dźwięk E na D9 |
| 6 | Dodatkowo oświetlenie F na D3 |
| 7 lub 8 | Jak 6; G już działa, H jest pomiarem sprzętowym |
| 9 | Wszystkie powyższe i LCD I |

Na demonstrację końcową ustaw APP_STAGE 9. Numer etapu to narzędzie do uruchamiania,
nie opcja menu dla prowadzącego.

## 2. Połączenia

Wyłącz zasilanie przed przepinaniem przewodów. Wszystkie moduły mają wspólną masę.

| Element | Sygnał | Uwagi |
|---|---|---|
| Temperature Sensor | A0 / PA_3 | Zachowaj obecne połączenie; zasilanie analogowego modułu 3,3 V |
| Light Sensor | A1 / PC_0 | Zachowaj; zasilanie 3,3 V |
| Rotary Angle Sensor | A2 / PC_3 | Zachowaj; zakres wyjścia nie przekracza 3,3 V |
| Button zewnętrzny | D2 / PF_15 | Wybór progu, domyślnie naciśnięcie = 1 |
| USER/B1 na płytce | PC_13 | Potwierdzenie alarmu; nie wymaga dodatkowego przewodu |
| LED oświetlenia | D3 / PE_13 | Osobny element wykonawczy, PWM TIM1 |
| Buzzer | D9 / PD_15 | Sygnał PWM TIM4; zasilanie modułu według specyfikacji |
| LCD SDA | D14 / PB_9 | I2C |
| LCD SCL | D15 / PB_8 | I2C |
| Sygnał pomiarowy opcjonalny | D7 / PF_13 | Tylko dla analizatora/oscyloskopu, ENABLE_PROBE=true |

Światełko widoczne na Light Sensor nie oznacza, że moduł zapewnia niezależnie
sterowaną lampę. A1 jest wejściem pomiarowym. Punkt F potrzebuje osobnej LED/modułu
LED na D3. Nie używaj podświetlenia LCD jako zamiennika tej lampy.

Zwykła mała LED: wyjście D3 -> rezystor ograniczający prąd -> anoda LED, katoda -> GND.
Przykład: 330 omów dla zwykłej czerwonej LED daje kilka mA przy 3,3 V; dobierz do
faktycznej LED. Dla gotowego modułu podłącz wejście SIG zgodnie z jego opisem.
Dla większego prądu użyj stopnia tranzystorowego, nie zasilaj obciążenia z GPIO.

Buzzer ma w otrzymanej specyfikacji zasilanie 5 V. To nie znaczy, że GPIO generuje
5 V: wyjście płytki jest 3,3 V. Dla modułu VCC/GND/SIG sprawdź akceptację 3,3 V
na SIG. Goły dwuprzewodowy buzzer wymagający większego prądu wymaga sterownika.
Bez modelu modułu nie można potwierdzić elektrycznej zgodności podłączenia.

LCD: sprawdź oznaczenia VCC/GND/SDA/SCL i wymagane napięcie Waszej wersji modułu;
nie zamieniaj SDA z SCL. Sprawdź też napięcie podciągania I2C na używanym module/shieldzie.
Nie podawaj 5 V na wejścia analogowe. Adresy biblioteki są w formacie Mbed 8-bit:
LCD 0x7C, RGB 0xC4 (odpowiednio 7-bit 0x3E i 0x62).

## 3. Podział na klasy

Każda klasa ma własny nagłówek .h oraz implementację .cpp.

| Klasa | Odpowiedzialność |
|---|---|
| TempSensor | ADC temperatury i wzór termistora; błędny odczyt -> NaN |
| LightSensor | Jedna próbka ADC światła na cykl |
| ThresholdController | Pokrętło i D2, filtr pokrętła, debounce wyboru |
| ThresholdModel | Mapowanie 10–40°C, krok 0,5°C, odstęp 2°C, przechwycenie pokrętła |
| MeasurementFilter | Filtr wykładniczy, pierwsza próbka bez startu od zera |
| AlarmController | Tylko logika stanów i histerezy, bez sprzętu |
| AlarmButton | InterruptIn, przekazanie zdarzenia, debounce, pojedyncze kliknięcie |
| BuzzerController | Ton PWM 2 kHz i rytm zależny od stanu |
| LightingController | Procent światła i PWM lampy 1 kHz |
| DisplayController | Formatowanie 16x2, strony, kolory, użycie Waszej biblioteki |
| Grove_LCD_RGB_Backlight | Dostarczona przez Was biblioteka I2C LCD |
| EnvironmentMonitor | Składa obiekty, harmonogram pomiarów i aktualizacja wyjść |

main.cpp tworzy EnvironmentMonitor wewnątrz main() i wywołuje run(). Dzięki temu
inicjalizacja LCD z sleep_for nie wykonuje się przed startem systemu.
AppConfig.h zawiera ustawienia i piny, a DisplayData jest strukturą przenoszącą
wyniki do wyświetlacza. Nie są sterownikami sprzętu.

## 4. A — szybkie sprawdzenie istniejących pomiarów

1. APP_STAGE=2. Uruchom program; w terminalu co 500 ms pojawiają się dane.
2. Sprawdź temperaturę w pokoju. Delikatnie ogrzej czujnik palcami — wynik powinien rosnąć.
3. Zasłoń i odsłoń czujnik światła. rawLight powinno zmieniać się w zakresie 0–1.
4. valid=1 oznacza poprawny zakres obliczeń temperatury. valid=0 oznacza błąd;
   przy etapach >=3 uruchomi alarm, a LCD pokaże TEMP ERROR.

Poprawka: skrajne wartości ADC nie są już prezentowane jako -273,15°C. Nie jest to
pełna diagnostyka wszystkich uszkodzeń czujnika — niektóre błędy mogą nadal dawać
pozornie wiarygodne napięcie. Wzór i beta=4250 pozostają z oryginalnego projektu.
Nie podajemy niezweryfikowanego wyniku w luksach: używamy procentowej skali światła.

## 5. B — dokończenie nastaw progów

### Co jest w kodzie

- Dolny próg startuje od 18°C, górny od 28°C.
- D2 puszczony: edytujesz LOW. D2 trzymany: edytujesz HIGH.
- Pokrętło obejmuje 10–40°C; nastawa ma krok 0,5°C.
- Dolny próg nie przekroczy HIGH-2°C, górny nie spadnie poniżej LOW+2°C.
- D2 musi utrzymać nowy stan przez 50 ms.
- Po starcie i zmianie edytowanego progu działa przechwycenie (soft takeover).
  PICK oznacza: obróć pokrętło do obecnej wartości wybranego progu lub przejdź przez nią.
  SET oznacza: pokrętło już zmienia nastawę. knob pokazuje temperaturę przypisaną jego pozycji.
- Nastawy są w RAM. Reset przywraca 18/28°C; zapis do pamięci nie był wymagany.

### Test krok po kroku

1. APP_STAGE=2, wgraj. Nie naciskaj D2. Oczekuj edit=LOW, L=18, H=28.
2. Obracaj powoli pokrętło, aż knob dojdzie do około 18. Zobaczysz SET.
3. Ustaw LOW np. na 19°C. HIGH ma nadal wynosić 28°C.
4. Przytrzymaj D2. Powinno być edit=HIGH i PICK. HIGH nie może przeskoczyć na 19°C.
5. Nadal trzymając D2, przejdź pokrętłem przez 28°C; potem ustaw HIGH np. na 30°C.
6. Puść D2. LOW nadal 19°C. Wróć pokrętłem do 19°C, aby ponownie go edytować.
7. Spróbuj podnieść LOW powyżej HIGH. Zatrzyma się na HIGH-2°C.
8. Spróbuj obniżyć HIGH poniżej LOW. Zatrzyma się na LOW+2°C.
9. Zresetuj płytkę. Progi znów 18 i 28; pokrętło ponownie czeka na przechwycenie.

Jeśli edit=HIGH pojawia się po puszczeniu zamiast po naciśnięciu, zmień EDIT_ACTIVE
z 1 na 0 w AppConfig.h. To wybiera również właściwe podciąganie wejścia.
Nie zmieniaj tej wartości, dopóki nie sprawdzisz rzeczywistego zachowania D2.

B zawiera również wymaganie wywołania alarmu po przekroczeniu progu. Nastawy
sprawdzasz w etapie 2; wykrywanie przekroczenia uruchamiasz w etapie 3,
a jego dźwięk w etapie 5. Nie ma trzech osobnych implementacji alarmu.

## 6. C — automat stanów

Ustaw APP_STAGE=3. Filtr i histereza są już aktywne (punkt G).
T oznacza temperaturę po filtracji, L i H — bieżące nastawy.

| Stan/przejście | Reguła |
|---|---|
| NORMAL -> WARNING | T<L lub T>H, jeśli nie zachodzi już ALARM |
| NORMAL/WARNING -> ALARM | T<=L-3 lub T>=H+3 |
| WARNING -> NORMAL | L+0,5 <= T <= H-0,5 |
| ALARM -> WARNING | L-2,5 <= T <= H+2,5, ale poza zakresem powrotu NORMAL |
| ALARM -> NORMAL | L+0,5 <= T <= H-0,5 |
| ALARM -> ACKNOWLEDGED | Poprawne kliknięcie USER od etapu 4 |
| ACKNOWLEDGED -> NORMAL | L+0,5 <= T <= H-0,5 |
| Błędny pomiar temperatury | ALARM, albo pozostaje ACK jeśli już potwierdzony |

W innym przypadku stan zostaje zachowany. ALARM ma pierwszeństwo nad WARNING.
ACK wycisza dany epizod aż do bezpiecznego powrotu, także jeśli temperatura dalej
się pogarsza; to świadomie przyjęta reguła. Nowy epizod po powrocie do NORMAL
ponownie alarmuje. Zmiana nastaw także wpływa na stan i może zakończyć alarm.

Przykład bez ogrzewania czujnika, gdy temperatura wynosi około 24°C:

1. LOW=18, HIGH=28 -> NORMAL.
2. Zmień HIGH na 23 -> WARNING.
3. Zmień HIGH na 20 -> ALARM (24 >= 20+3).
4. Zmień HIGH na 28 -> NORMAL.
5. Sprawdź stronę dolną: HIGH=32 i LOW=25 -> WARNING; LOW=27 -> ALARM,
   jeśli rzeczywista T<=24. Wartości dostosuj do aktualnego pomiaru.

Obserwuj terminal i diody płytki: NORMAL -> LED1, WARNING -> LED2,
ALARM -> LED3, ACK -> LED2+LED3. LED2 płytki jest niebieska, a nie żółta;
żółta sygnalizacja WARNING pojawia się dopiero na podświetleniu LCD.

## 7. D — potwierdzenie przez przerwanie

1. Ustaw APP_STAGE=4. D2 nadal wybiera próg. USER/B1 na płytce obsługuje ACK.
2. Doprowadź do ALARM według testu C.
3. Naciśnij USER na co najmniej 50 ms. Stan ma zmienić się na ACK.
4. Pozostaw nieprawidłową temperaturę: ACK ma pozostać widoczny.
5. Przywróć zakres bezpieczny: NORMAL. Wywołaj nowe przekroczenie: znów ALARM.
6. Naciśnij USER w NORMAL i trzymaj podczas wywołania alarmu. Nie powinien
   automatycznie potwierdzić nowego alarmu; puść na >=50 ms i naciśnij ponownie.

InterruptIn jest na PC_13, domyślnie rise (naciśnięcie=1). Przerwanie ustawia tylko
flagę. Pętla atomowo odbiera flagę w sekcji krytycznej, sprawdza przycisk i czas.
Nie ma LCD, printf, obliczeń ani czekania wewnątrz przerwania.
Wyciszenie po zaakceptowaniu przycisku następuje przy kolejnej obsłudze pętli,
bez czekania na następny pomiar temperatury. Rzeczywisty czas zmierz w H.

## 8. E — dźwięk

1. Przy wyłączonym zasilaniu podłącz odpowiedni moduł buzzera, SIG=D9.
2. Ustaw APP_STAGE=5 i wgraj program.
3. NORMAL: cisza.
4. WARNING: 100 ms tonu, 900 ms ciszy.
5. ALARM: 150 ms tonu, 150 ms ciszy.
6. USER -> ACK: cisza mimo utrzymującego się przekroczenia.
7. Podczas pikania sprawdź, że nadal można zmieniać nastawy i odczytywać czujniki.

Nośna tonu: PWM 2 kHz (500 us), wypełnienie 50% podczas pisku i 0% podczas ciszy.
Rytm jest wyliczany z zegara, nie blokuje aplikacji długim sleep_for.
Jeśli buzzer jest aktywny (własny generator), jego zachowanie może różnić się od
pasywnego modułu opisanego na zajęciach. Najpierw sprawdź model i połączenia.

## 9. F — automatyczne oświetlenie PWM

1. Podłącz osobną LED z rezystorem/moduł lampy do D3.
2. Ustaw APP_STAGE=6. Zasłoń czujnik A1: lampa powinna się rozjaśnić.
3. Oświetl czujnik: lampa powinna przygasnąć.
4. W terminalu porównaj light i LED. Powinny sumować się do 100%.
5. Dla 80% światła lampę steruje 20% PWM; dla 0% — 100%, dla 100% — 0%.

PWM lampy: 1 kHz (1000 us). Nie myl częstotliwości z wypełnieniem.
Przy module o wejściu aktywnym niskim potrzebna będzie inwersja sterowania;
przyjęto zwykłą LED/moduł aktywny wysokim poziomem.

Kalibracja:

1. Odczytaj rawLight przy docelowej ciemności, np. 0,08.
2. Odczytaj rawLight przy docelowym jasnym otoczeniu, np. 0,82.
3. Wpisz własne wyniki w LIGHT_DARK i LIGHT_BRIGHT w AppConfig.h. To przykłady,
   nie pomiary Waszego sprzętu. Jasny punkt musi być większy od ciemnego.
4. Ponownie wgraj. Wynik jest ograniczany do 0–100%.

Przed kalibracją wartości 0 i 1 oznaczają procent pełnego zakresu ADC.
W trakcie kalibracji ogranicz wpływ lampy na czujnik (np. skieruj lampę w inną stronę).
W demonstracji zadbaj o stabilną geometrię, żeby samo oświetlenie LED nie powodowało
ciągłych zmian pomiaru. Zależność procentowa spełnia regułę ćwiczenia, ale nie jest
kalibrowanym regulatorem stałej liczby luksów.

## 10. G — filtr i histereza

Są aktywne od APP_STAGE=3. Filtr temperatury i światła aktualizuje się przy pomiarze
nominalnie co 100 ms. Pokrętło ma własny filtr aktualizowany w pętli.

    filtered = filtered + 0.2 * (sample - filtered)

Pierwsza próbka inicjalizuje filtr bez sztucznego startu od zera.
Filtr wygładza pomiar, a histereza oddziela próg wejścia i wyjścia ze stanu.
Dla HIGH=28: WARNING zaczyna się powyżej 28, powrót NORMAL przy <=27,5
(o ile temperatura jest też >=LOW+0,5). ALARM wchodzi od 31, a opuszcza górną
strefę alarmową przy <=30,5. Analogicznie po stronie dolnej.

Test: ustaw próg blisko temperatury i obserwuj przez 30–60 s. Stan nie powinien
przeskakiwać przy każdym drobnym wahnięciu. Przy realnym ogrzewaniu uwzględnij
opóźnienie czujnika i filtra. Nie oceniaj progów na podstawie nieprzefiltrowanej temperatury.

## 11. H — pomiary na stanowisku

To czynności sprzętowe, których kod sam nie zastąpi. Użyj oscyloskopu lub analizatora
logicznego i wypełnij docs/POMIARY_H.csv. Nie zmieniaj kolumny „wynik” na podstawie
samej wartości w kodzie. Przy braku przyrządu uzgodnij sposób walidacji z prowadzącym.

1. Zmierz D3 względem GND: okres około 1 ms, częstotliwość około 1 kHz.
2. Dla wypełnienia 20%, 50%, 80% zmierz czas HIGH: około 0,2/0,5/0,8 ms.
   Najprościej dobrać oświetlenie czujnika do wartości z terminala; zanotuj faktyczny
   zadany procent i porównaj z pomiarem, zamiast zakładać idealne 20/50/80.
3. Zmierz D9 podczas tonu: okres około 0,5 ms; HIGH około 0,25 ms.
4. Zmierz obwiednię D9: WARNING 100/900 ms, ALARM 150/150 ms.
5. ENABLE_PROBE=true włącza impuls na D7 podczas pomiarów. Zmierz odstęp początków
   impulsów: nominalnie 100 ms. Szerokość impulsu odpowiada blokowi pomiarów,
   przeliczeń i aktualizacji lampy, a nie samemu ADC.
6. Zarejestruj USER/PC13 i D9 podczas ALARM. Zmierz czas od naciśnięcia do wyciszenia;
   uwzględnij debounce 50 ms i fakt, że naciśnięcie podczas przerwy dźwięku nie daje
   bezpośredniej krawędzi „wyłączenia”. Do tego pomiaru naciskaj podczas tonu.
7. Powtórz sprawdzenie rytmów przy APP_STAGE=9 z LCD, a nie tylko bez wyświetlacza.
8. Po zakończeniu możesz przywrócić ENABLE_PROBE=false.

Harmonogram jest współpracujący: sleep_for(10 ms), I2C i printf mają narzut.
100/150 ms to wartości docelowe, nie gwarancja twardego czasu rzeczywistego.
Nie dopisujemy arbitralnej tolerancji jako wymagania prowadzącego: zanotuj wyniki,
rozrzut, ustawienia i wyjaśnij odchylenia. Terminal zablokowany lub intensywny log
może wpływać na czasy, dlatego końcowy test wykonaj też w warunkach demonstracji.

## 12. I — Wasza biblioteka LCD

1. Biblioteka już jest w lib/Grove_LCD_RGB_Backlight; nie dodawaj drugiej kopii.
2. Podłącz LCD: SDA=D14, SCL=D15, VCC/GND według modułu.
3. Ustaw APP_STAGE=9 i wgraj.
4. Pierwszy wiersz stale pokazuje temperaturę i stan. Przy błędzie: TEMP ERROR.
5. Drugi wiersz zmienia stronę co 2 s: światło i LED; LOW/HIGH; edytowany próg,
   PICK/SET i pozycja pokrętła K.
6. Sprawdź podświetlenie: zielony NORMAL, żółty WARNING, czerwony ALARM, fioletowy ACK.
7. Sprawdź możliwość ustawienia progów i wyciszenia alarmu bez terminala.

Tekst jest aktualizowany nominalnie co 250 ms. Każdy wiersz jest dopełniany spacjami
zamiast clear() w każdej iteracji, żeby uniknąć migotania i resztek dłuższych napisów.
Biblioteka korzysta z adresu RGB 0x62 (7-bit). Nowsze wersje Grove mogą mieć inny
kontroler; jeśli tekst działa, ale kolory nie, sprawdź wersję modułu. Nie zmieniaj
samego adresu w ciemno — układ może mieć także inny zestaw rejestrów.
Dostarczona biblioteka nie raportuje błędów I2C; brak wyświetlania wymaga sprawdzenia
połączeń/adresów na sprzęcie. W tej paczce nie deklarujemy automatycznej detekcji LCD.

W PDF jest „Show the date”, ale sekcja Data log wymienia temperaturę i progi.
Implementacja pokazuje wymagane dane systemu, nie datę kalendarzową. Jeżeli
prowadzący dosłownie wymaga daty, trzeba ustalić sposób nastawiania RTC i utrzymania
czasu; tej niejednoznaczności nie rozwiązujemy fikcyjną datą z kompilacji.

## 13. Co dokładnie zmieniono

- main.cpp: tylko utworzenie i uruchomienie aplikacji.
- TempSensor: sprawdzenie krańców ADC, NaN zamiast fałszywej temperatury.
- LightSensor: jeden odczyt raw, usunięte niezweryfikowane getLux i podwójne odczyty.
- ThresholdController: debounce, filtr pokrętła, delegowanie reguł do ThresholdModel.
- ThresholdModel: zakres, krok, odstęp 2°C i przechwycenie pozycji.
- Dodane klasy automatu, filtra, przycisku, dźwięku, oświetlenia, LCD i aplikacji.
- Konfiguracja etapów i pinów w AppConfig.h.
- Dodana Wasza biblioteka LCD. W niej: pragma once, stan wyświetlacza jako pola
  obiektu zamiast globalnych zmiennych, font 5x8 dla 16x2, jawne 100 kHz I2C,
  jawny typ char dla 0x80 oraz zapis chrono niezależny od using namespace.
  API konstruktora/setRGB/print/locate pozostaje takie samo.
- mbed_app.json i mbed-os.lib pozostają z repozytorium.
- Dodane testy hostowe i .mbedignore, instrukcja oraz pusta tabela pomiarów H.

## 14. Weryfikacja i granice wyniku

Uruchomienie na Linux: bash tests/run_host_tests.sh (wymaga g++).
Testy sprawdzają automat, oba kierunki histerezy, ACK i ponowne uzbrojenie, błędny ADC,
10 000 zmian nastaw z zachowaniem ograniczeń, filtr, rytmy buzzera, PWM światła,
debounce D2/USER i długości wierszy LCD. Sprawdzana jest składnia etapów 2,3,4,5,6,9.
To testy z atrapą Mbed HAL, nie symulacja rzeczywistego MCU ani build ARM.
Build w Mbed Studio i wszystkie czynności H pozostają po Waszej stronie.

## 15. Odbiór końcowy

- APP_STAGE=9, brak błędów kompilacji dla NUCLEO_F767ZI.
- Na LCD odczyty, progi, edytowany parametr i stan; progi dają się ustawić bez PC.
- NORMAL/WARNING/ALARM po obu stronach zakresu działają zgodnie z tabelą.
- USER potwierdza alarm, ale nie usuwa wizualnego ostrzeżenia.
- Po powrocie do normy następny alarm ponownie uruchamia dźwięk.
- LED zwiększa jasność przy spadku światła otoczenia.
- Tabela H zawiera rzeczywiste wyniki.
- Samodzielny test bez terminala, przy odpowiednim zasilaniu.
- Usuń z kopii do oddania ewentualne katalogi kompilacji i dane lokalne.
- Nazwa z PDF: AME26_27_DEVELOPER1_DEVELOPER2_MANDATIRY_EXERCISE1.zip
  (zachowano pisownię MANDATIRY z polecenia). Wstaw Wasze nazwiska/identyfikatory.
- Termin w PDF: 20 listopada 2026, 23:55; zweryfikuj strefę i zadanie w CampusVirtual.
