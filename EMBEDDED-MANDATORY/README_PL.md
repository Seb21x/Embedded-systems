# Zmiany A–F względem pierwotnego projektu

Baza: Seb21x/Embedded-systems, commit 5044c333e318d36ebe33b3886af49cdd0b17e7c8.
C++ i Mbed OS; target NUCLEO_F767ZI. Bez LCD, bez wyboru etapów i bez przechwytywania
pokrętła. Każdy nowy sterownik jest osobną klasą w plikach .h oraz .cpp.

## Podłączenia

- A0: Temperature Sensor.
- A1: Light Sensor.
- A2: Rotary Angle Sensor.
- D2: Button wybierający próg.
- D3: moduł Grove LED Socket Kit.
- D5: moduł Grove Buzzer.
- Niebieski USER/B1 na płytce (PC13): potwierdzenie alarmu, nic nie dopinać.

D3 i D5 korzystają z TIM1. Obydwa wyjścia mają ten sam okres 500 us (2 kHz).
Nie zmieniaj częstotliwości jednego z nich niezależnie od drugiego.
Zdjęcia pokazują moduły Grove, ale nie potwierdzają napięcia przełącznika nakładki.
Zachowaj zasilanie czujników analogowych zgodne z ich wymaganiami; nie przełączaj
całej nakładki na 5 V tylko dla buzzera. LED musi być zamontowana w module LED Socket.

## Wgranie

1. Zrób kopię projektu.
2. Zastąp folder EMBEDDED-MANDATORY folderem z paczki. Zachowaj lokalnie pobrany
   mbed-os, jeśli jest potrzebny; archiwum zawiera mbed-os.lib, nie cały system.
3. Nie kopiuj tej wersji na poprzednią rozbudowaną wersję B–I: pozostałe pliki
   mogą również zostać skompilowane. Użyj czystego folderu z tej paczki.
4. Otwórz w Mbed Studio, wybierz NUCLEO_F767ZI, Build i wgraj.
5. Otwórz terminal tak jak w Waszej pierwotnej wersji.

Alternatywa: plik ZMIANY_A-F.patch można zastosować poleceniem
`git apply ZMIANY_A-F.patch` w katalogu głównym repozytorium na wskazanej bazie.
Nie stosuj jednocześnie patcha i zastępowania plików.

## Co zmieniono

A: zachowano klasy TempSensor i LightSensor. Skrajne ADC temperatury zwracają NaN;
   aplikacja sygnalizuje błąd przez valid=0 i alarm. Stare getLux pozostaje dla
   zgodności, ale nie jest używane: sterujemy procentem zakresu ADC, nie luksami.
B: zachowano ThresholdController. Zakres 10–40°C, krok 0,5°C, odstęp progów >=2°C.
   D2 puszczony -> LOW; trzymany -> HIGH. Wybrany próg od razu śledzi pokrętło,
   także po puszczeniu przycisku. To prosta obsługa jak w pierwotnym kodzie.
   Początkowe 18/28°C są nadpisywane dla wybranego progu przy pierwszym odczycie.
C: dodano AlarmController. NORMAL -> WARNING po wyjściu poza LOW/HIGH;
   ALARM przy T<=LOW-3 lub T>=HIGH+3. Minimalna histereza 0,5°C zapobiega
   miganiu stanów (wymagana również w opisie automatu). Powrót NORMAL wymaga
   LOW+0,5 <= T <= HIGH-0,5. ALARM schodzi do WARNING po powrocie między
   LOW-2,5 a HIGH+2,5, chyba że już osiągnięto bezpieczny zakres NORMAL.
D: AlarmButton używa InterruptIn na PC13, rise, debounce 50 ms.
   USER w ALARM -> ACKNOWLEDGED. Ostrzeżenie wizualne pozostaje, dźwięk milknie.
   ACK trwa do powrotu NORMAL. Kolejny epizod może ponownie alarmować.
   Kliknięcia poza ALARM są odrzucane. ISR ustawia tylko flagę.
E: BuzzerController: WARNING 100 ms tonu / 900 ms ciszy,
   ALARM 150 ms tonu / 150 ms ciszy, NORMAL i ACK bez dźwięku.
F: LightingController: PWM LED = 1 - lightPercent/100, ograniczone do 0–1.
   80% światła -> 20% LED. To skala ADC bez kalibracji fizycznych luksów.

main.cpp składa klasy, pobiera pomiary nominalnie co 100 ms, wypisuje wyniki
co 500 ms i obsługuje przycisk oraz buzzer w krótkiej pętli z pauzą 10 ms.
Nie dodano filtra pomiarów, LCD ani funkcji z późniejszych punktów.

## Sygnalizacja

NORMAL: zielona LED1, cisza.
WARNING: niebieska LED2, wolny dźwięk.
ALARM: czerwona LED3, szybki dźwięk.
ACK: LED2 i LED3, cisza.
Osobna LED D3 cały czas uzupełnia światło i nie jest diodą stanu alarmu.

## Test na płytce

1. Ustaw LOW i HIGH tak, aby temperatura była pomiędzy nimi. Oczekuj NORMAL.
2. Gdy T jest około 24°C: LOW=18, HIGH=23 -> WARNING; HIGH=20 -> ALARM.
   Uwaga: po puszczeniu D2 zaczynasz zmieniać LOW obecną pozycją pokrętła.
3. W ALARM naciśnij USER -> ACK i cisza. Przywróć progi obejmujące temperaturę
   z zapasem 0,5°C -> NORMAL. Powtórz przekroczenie -> nowy dźwięk.
4. Sprawdź także przekroczenie dolnego progu.
5. Zasłoń A1 -> LED D3 jaśniejsza; oświetl A1 -> LED ciemniejsza.
6. Jeśli wybór HIGH działa przy puszczonym D2: sprawdź polaryzację modułu;
   obecny kod zakłada 1 przy naciśnięciu, tak jak pierwotna wersja.

## Sprawdzenie wykonane przed przekazaniem

Przeszła kontrola składni g++ C++14 (-Wall -Wextra -Werror) z atrapą Mbed HAL.
Przeszły testy automatu, histerezy, ACK, ograniczeń progów, PWM, krańców ADC
oraz przycisku. Testy nie zastępują kompilacji ARM/Mbed ani uruchomienia na płytce.
Nie wykonano flashowania ani fizycznych pomiarów. Zmian nie wysłano na GitHub.
