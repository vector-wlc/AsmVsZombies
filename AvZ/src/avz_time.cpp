/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 10:22:46
 * @Description: old time
 */

#include "avz.h"

void AvZ::ice3(int time)
{
	delay(time - 95);
	insertOperation([=]() {
		auto ice_plant = main_object->plantArray();
		for (int index = 0; index < main_object->plantCountMax(); ++index, ++ice_plant)
		{
			if (!ice_plant->isDisappeared() && !ice_plant->isCrushed() &&
				ice_plant->type() == HBG_14 && ice_plant->state() == 2)
			{
				ice_plant->explodeCountdown() = 96;
				return;
			}
		}
	});
}