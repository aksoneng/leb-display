#include "leb-display.h"

SoftwareSerial SoftSerial(D5, D6);
#define SEND(x) SoftSerial.write(x); checksum = checksum ^ x;

// PUBLIC
leb_display::leb_display(char panel_id, int pin_srx, int pin_stx, int pin_re, int pin_de, int time_len, int symbol_len, int text_len) {

    _panel_id =panel_id;
    _time_len =time_len;
    _symbol_len =symbol_len;
    _text_len =text_len;

    _pin_srx =pin_srx;
    _pin_stx =pin_stx;
    _pin_re =pin_re;
    _pin_de =pin_de;

    SoftSerial.begin(9600);

    pinMode(_pin_re, OUTPUT);
    pinMode(_pin_de, OUTPUT);
}

void leb_display::display(char d_time[], char d_text1[], char d_text2[]){
    int j =0;
    while (!sendTime(d_time)) {
        delay(_SEND_DELAY );
        j++;
    }
    if( j >=1) {
        Serial.print("sendTime attempts: ");
        Serial.println(j);
    }

    j =0;
    while (!sendText(1, d_text1)) {
        delay(_SEND_DELAY );
        j++;
    }
    if( j >=1) {
        Serial.print("sendText 1 attempts: ");
        Serial.println(j);
    }

    j =0;
    while (!sendText(2, d_text2)) {
        delay(_SEND_DELAY );
        j++;
    }
    if( j >=1) {
        Serial.print("sendText 2 attempts: ");
        Serial.println(j);
    }
}

void leb_display::clean(){
    cleanTime();
    // cleanSymbol();
    cleanText();
}


// PRIVATE
void leb_display::cleanTime(){
    // Serial.println("Cleaning Time..");
    int j =0;
    char CLEAN[4 +1] = {"    "};
    while (!sendTime(CLEAN)) {
        delay(_SEND_DELAY );
        j++;
    }
    if( j >=1) {
        Serial.print("cleanTime attempts: ");
        Serial.println(j);
    }
}

void leb_display::cleanSymbol(){
    int j =0;
    char CLEAN[2 +1] = {"  "};
    while (!sendTime(CLEAN)) {
        delay(_SEND_DELAY );
        j++;
    }
    if( j >=1) {
        Serial.print("cleanSymbol attempts: ");
        Serial.println(j);
    }
}

void leb_display::cleanText(){
    int j =0;
    char CLEAN[17 +1] = {"                 "};

    while (!sendText(1, CLEAN)) {
        delay(_SEND_DELAY );
        j++;
    }
    if( j >=1) {
        Serial.print("1. cleanText attempts: ");
        Serial.println(j);
    }

    j =0;
    while (!sendText(2, CLEAN)) {
        delay(_SEND_DELAY );
        j++;
    }
    if( j >=1) {
        Serial.print("2. cleanText attempts: ");
        Serial.println(j);
    }
}


char leb_display::sendDebutPhase2(char command) {

    char checksum = 0;

    digitalWrite(_pin_de, HIGH);

    digitalWrite(_pin_re, HIGH);

    SoftSerial.write(0x02);

    SEND(0x30); SEND(0x30 + command);

    return checksum;

}

char *leb_display::padRight(char *string, int padded_len) {
    char format[6] ="%-";
    char buffer[3];
    sprintf(buffer, "%d", padded_len);
    strcat(format, buffer);
    strcat(format, "s");

    sprintf(string, format, string);
    return string;
}


int leb_display::sendPhase1(int i) {

    digitalWrite(_pin_re, HIGH); digitalWrite(_pin_de, HIGH);
    SoftSerial.write(0x40 + i);
    SoftSerial.write(0x05);
    SoftSerial.flush();
    digitalWrite(_pin_re, LOW); digitalWrite(_pin_de, LOW);

    int r1 =0;
    int r2 =0;
    int r3 =0;

    int x = 0;

    while (!SoftSerial.available() && x < _SERIAL_ATTEMPTS) {
        delay(_SERIAL_DELAY);
        x++;
    }
    if( x >=_SERIAL_ATTEMPTS) {
        Serial.print("1. SoftSerial retries: ");
        Serial.println(x);
    }
    r1 = SoftSerial.read();


    x = 0;
    while (!SoftSerial.available() && x < _SERIAL_ATTEMPTS) {
        delay(_SERIAL_DELAY);
        x++;
    }
    if( x >=_SERIAL_ATTEMPTS) {
        Serial.print("2. SoftSerial retries: ");
        Serial.println(x);
    }
    r2 = SoftSerial.read();

    // SoftSerial.println(r2, HEX);
    if (r2 != 0x90) {
        Serial.print("2. r2: ");
        Serial.println(r2);
        return 0;
    }
    x = 0;
    while (!SoftSerial.available() && x < _SERIAL_ATTEMPTS) {
        delay(_SERIAL_DELAY);
        x++;
    }
    if( x >=_SERIAL_ATTEMPTS) {
        Serial.print("3. SoftSerial retries: ");
        Serial.println(x);
    }
    r3 = SoftSerial.read();

    if(r3 != 0x30) {
        Serial.print("3. r3: ");
        Serial.println(r3);
    }

    return r3 == 0x30;
}

int leb_display::sendTime(char * txt) {

    if (!sendPhase1(_panel_id))
    return 0;

    char checksum = sendDebutPhase2(0x02);

    SEND(0x30); SEND(0x30 + 1);
    SEND(0x30); SEND(0x30);

    for (int i = 0; i < _time_len; i++) {
        SEND(0x30 + ((txt[i] >> 4) & 0xF)); SEND(0x30 + (txt[i] & 0xF));
    }
    SEND(0x30); SEND(0x30);
    SEND(0x30); SEND(0x30);

    SEND(0x30); SEND(0x3D);
    SEND(0x30); SEND(0x30);

    return sendFinPhase2(checksum);

}

int leb_display::sendText(int id, char * txt) {

    txt =padRight(txt, _text_len);

    if (!sendPhase1(_panel_id))
    return 0;

    char checksum = sendDebutPhase2(0x00);

    SEND(0x30); SEND(0x30 + id);
    SEND(0x30); SEND(0x30);

    for (int i = 0; i <_text_len; i++) {
        SEND(0x30 + ((txt[i] >> 4) & 0xF)); SEND(0x30 + (txt[i] & 0xF));
    }
    SEND(0x30); SEND(0x30);
    SEND(0x30); SEND(0x30);

    SEND(0x30); SEND(0x3D);
    SEND(0x30); SEND(0x30);

    return sendFinPhase2(checksum);

}

int leb_display::sendSymbol(char * txt) {

    if (!sendPhase1(_panel_id))
    return 0;

    char checksum = sendDebutPhase2(0x01);

    SEND(0x30); SEND(0x30 + 1);
    SEND(0x30); SEND(0x30);

    for (int i = 0; i < _symbol_len; i++) {
        SEND(0x30 + ((txt[i] >> 4) & 0xF)); SEND(0x30 + (txt[i] & 0xF));
    }
    SEND(0x30); SEND(0x30);
    SEND(0x30); SEND(0x30);

    SEND(0x30); SEND(0x3D);
    SEND(0x30); SEND(0x30);

    return sendFinPhase2(checksum);

}

int leb_display::sendFinPhase2(char checksum) {

    SEND(0x03);

    SoftSerial.write(checksum);
    SoftSerial.flush();
    digitalWrite(_pin_re, LOW); digitalWrite(_pin_de, LOW);

    int r2 =0;
    int r3 =0;

    int x = 0;
    while (!SoftSerial.available() && x < _SERIAL_ATTEMPTS) {
        delay(_SERIAL_DELAY);
        x++;
    }
    if( x >=_SERIAL_ATTEMPTS) {
        Serial.print("4. SoftSerial retries: ");
        Serial.println(x);
    }
    r2 = SoftSerial.read();


    if (r2 != 0x90)
    {
        Serial.print("4. r2: ");
        Serial.println(r2);
        return 0;
    }

    x = 0;
    while (!SoftSerial.available() && x < _SERIAL_ATTEMPTS) {
        delay(_SERIAL_DELAY);
        x++;
    }
    if( x >=_SERIAL_ATTEMPTS) {
        Serial.print("5. SoftSerial retries: ");
        Serial.println(x);
    }
    r3 = SoftSerial.read();
    if(r3 != 0xB1) {
        Serial.print("5. r3: ");
        Serial.println(r3);
    }

    return r3 == 0xB1;

}
