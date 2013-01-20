/*
 * NFC-Fのカードをかざすと、ボード上のLEDが点灯します。
 */

#include <HkNfcRw.h>
#include <inttypes.h>
#include <string.h>


#define LED_PIN 13

void setup()
{
	bool ret;

	digitalWrite(LED_PIN, LOW);
	pinMode(LED_PIN, OUTPUT);

	ret = HkNfcRw::open();
	while (!ret) {}
}

void loop()
{

	HkNfcRw::Type type = HkNfcRw::detect(true, true, true);
	//NFC_Aにすると点滅しているので、よくわからんが失敗もしているみたい。
	if(type == HkNfcRw::NFC_F) {
		digitalWrite(LED_PIN, HIGH);
	} else {
		digitalWrite(LED_PIN, LOW);
	}

	delay(500);
}
