/*
 * SNEP PUT(initiator)で、http://www.google.co.jp/ のURIを飛ばします。
 * 成功すると、ボード上のLEDが1秒間隔で点灯したままで止まります(無限ループ)。
 * 失敗した場合は、それより短い周期で点滅します。
 * まだライブラリの作りが甘いので、変だったらRESETボタンを押してください。
 */

#include <HkNfcRw.h>
#include <HkNfcSnep.h>
#include <HkNfcNdef.h>


#define PIN_LED		(13)

static HkNfcNdefMsg s_Msg;

static void Abort(int wait)
{
	while(1) {
		digitalWrite(PIN_LED, HIGH);
		delay(wait);
		digitalWrite(PIN_LED, LOW);
		delay(wait);
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

	pinMode(PIN_LED, OUTPUT);
	digitalWrite(PIN_LED, LOW);

	ret = HkNfcRw::open();
	if(!ret) {
		Abort(50);
	}

	ret = HkNfcNdef::createUrl(&s_Msg, HkNfcNdef::HTTP_WWW, "google.co.jp/");
	if(!ret) {
		Abort(50);
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
			Abort(1000);
		} else {
			HkNfcRw::reset();
			Wait(50);
		}
	} else {
		HkNfcRw::reset();
		Wait(200);
	}
}
