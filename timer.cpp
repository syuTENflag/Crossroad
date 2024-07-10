#include "timer.h"
#include "DxLib.h"

timer::timer() {
	initialize();
}

void timer::initialize() {
	time_start = GetNowCount();		// Œ»İŒo‰ßŠÔ‚ğ“¾‚é
	time = 0.0f;					// Œ»İŠÔ‚Ì‰Šú‰»
}

void timer::update() {
	// ŠJn‚ÆŒ»İ‚Ì·‚ğŠi”[‚·‚é
	time = (GetNowCount() - time_start) / 1000;
}

void timer::draw() {
	// Œo‰ßŠÔ‚ğ•`‰æ
	DrawFormatString(250, 10, GetColor(255, 100, 100), "%d•b", time);
}
