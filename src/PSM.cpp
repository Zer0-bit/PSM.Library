#include "Arduino.h"
#include "PSM.h"

PSM *_thePSM;

PSM::PSM(unsigned char sensePin, unsigned char controlPin, unsigned int range)
{
	_thePSM = this;

	pinMode(sensePin, INPUT_PULLUP);
	PSM::_sensePin = sensePin;

	pinMode(controlPin, OUTPUT);
	PSM::_controlPin = controlPin;

	attachInterrupt(digitalPinToInterrupt(PSM::_sensePin), onInterrupt, RISING); // low-to-high transition = start of negative half-wave

	PSM::_range = range;
}

void PSM::onInterrupt()
{
	_thePSM->calculateSkip();
}

void PSM::set(unsigned int value)
{
	PSM::_value = value;
}

long PSM::getCounter()
{
	return PSM::_counter;
}

void PSM::resetCounter()
{
	PSM::_counter = 0;
}

void PSM::stopAfter(long counter)
{
	PSM::_stopAfter = counter;
}

void PSM::calculateSkip()
{
	PSM::_a += PSM::_value;

	if (PSM::_a >= PSM::_range)
	{
		PSM::_a -= PSM::_range;
		PSM::_skip = false;
	}
	else
	{
		PSM::_skip = true;
	}

	if (PSM::_a > PSM::_range)
	{
		PSM::_a = 0;
		PSM::_skip = false;
	}

	if (!PSM::_skip)
	{
		PSM::_counter++;
	}

	if (!PSM::_skip 
		&& PSM::_stopAfter > 0
		&& PSM::_counter > PSM::_stopAfter)
	{
		PSM::_skip = true;
	}

	updateControl();
}

void PSM::updateControl()
{
	if (PSM::_skip)
	{
		digitalWrite(PSM::_controlPin, LOW);
	}
	else
	{
		digitalWrite(PSM::_controlPin, HIGH);
	}
}
