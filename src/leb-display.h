#ifndef __LEB_DISPLAY_H
#define __LEB_DISPLAY_H

#include <SoftwareSerial.h>
#include <Arduino.h>

#define _SEND_DELAY      20
#define _SERIAL_DELAY    20
#define _SERIAL_ATTEMPTS 50

class leb_display{
public:
	leb_display(char panel_id, int pin_srx, int pin_stx, int pin_re, int pin_de, int time_len, int symbol_len, int text_len);
	void display(char d_time[], char d_text1[], char d_text2[]);
	void clean();
    void cleanTime();
    void cleanSymbol();
    void cleanText();

private:
    char sendDebutPhase2(char command);
    char *padRight(char *string, int padded_len);

	int sendPhase1(int i);
	int sendTime(char * txt);
	int sendText(int id, char * txt);
    int sendSymbol(char * txt);
    int sendFinPhase2(char checksum);

	char _panel_id;
    int _time_len;
    int _symbol_len;
    int _text_len;
    int _pin_srx;
    int _pin_stx;
    int _pin_re;
    int _pin_de;
};

#endif
