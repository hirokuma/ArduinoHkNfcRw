#include <HkNfcRw.h>
#include <HkNfcSnep.h>
#include <HkNfcNdef.h>
#include <inttypes.h>
#include <string.h>


#define PIN_LED		(13)
#define PIN_PCD		(8)

static HkNfcNdefMsg s_Msg;

static void Abort()
{
	while(1) {
		digitalWrite(PIN_LED, HIGH);
		delay(1000);
		digitalWrite(PIN_LED, LOW);
		delay(1000);
	}
}

static void Abort2()
{
	while(1) {
		digitalWrite(PIN_LED, HIGH);
		delay(50);
		digitalWrite(PIN_LED, LOW);
		delay(50);
	}
}

static void Wait(int wait)
{
	int i;

	for(i=0; i<5; i++) {
		digitalWrite(PIN_LED, HIGH);
		delay(wait);
		digitalWrite(PIN_LED, LOW);
		delay(wait);
	}

	delay(2000);
}

void setup()
{
	bool ret;

	pinMode(PIN_PCD, OUTPUT);
	digitalWrite(PIN_PCD, LOW);
	pinMode(PIN_LED, OUTPUT);
	digitalWrite(PIN_LED, LOW);
	delay(200);
	digitalWrite(PIN_PCD, HIGH);
	delay(200);

	ret = HkNfcRw::open();
	if(!ret) {
		Abort2();
	}

	ret = HkNfcNdef::createUrl(&s_Msg, HkNfcNdef::HTTP_WWW, "google.co.jp/");
	if(!ret) {
		Abort2();
	}
}

void loop()
{
	bool b;

	b = HkNfcSnep::putStart(HkNfcSnep::MD_INITIATOR, &s_Msg);
	if(b) {
		while(HkNfcSnep::poll()) {
			;
		}
		if(HkNfcSnep::getResult() == HkNfcSnep::SUCCESS) {
			Abort();
		} else {
			HkNfcRw::reset();
			Wait(50);
		}
	} else {
		HkNfcRw::reset();
		Wait(200);
	}
}
