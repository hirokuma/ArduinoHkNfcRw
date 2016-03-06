/*
 * NFC-Fのカードをかざすと、ボード上のLEDが点灯します。
 */

#include <HkNfcRw.h>
#include <HkNfcA.h>
#include <inttypes.h>


#define LED_PIN 13    //Uno OnBoard LED

void setup()
{
	bool ret;

pinMode(LED_PIN, OUTPUT);
digitalWrite(LED_PIN, LOW);
	
	ret = HkNfcRw::open();
	while (!ret) {}
}

void loop()
{

	HkNfcRw::Type type = HkNfcRw::detect(true, false, false);
	//NFC_Aにすると点滅しているので、よくわからんが失敗もしているみたい。
	if(type == HkNfcRw::NFC_A) {
    uint8_t buf[4];

    bool b = HkNfcA::read(buf, 3);
    if (b && (buf[0] == 0xe1)) {
      //CC0はNDEFを示している(NDEFタグかどうかはわからない)
      b = HkNfcA::read(buf, 6);
      //手元にあったタグを識別できるかどうか
      if (b && (buf[0] == 0x02) && (buf[1] == 0x6c) && (buf[2] == 0x53) && (buf[3] == 0x70)) {
    		digitalWrite(LED_PIN, HIGH);
      }
    }
	} else {
		digitalWrite(LED_PIN, LOW);
	}

	delay(500);
}
