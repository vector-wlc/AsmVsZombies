/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 10:22:46
 * @Description: old click
 */

#include "libavz.h"

void AvZ::clickGrid(int row, float col, int offset)
{
	int x, y;
	x = 80 * col;

	if ((main_object->scene() == 2) || (main_object->scene() == 3))
	{
		y = 55 + 85 * row + offset;
	}
	else if ((main_object->scene() == 4) || (main_object->scene() == 5))
	{
		if (col > 5)
			y = 45 + 85 * row + offset;
		else
			y = 45 + 85 * row + (120 - 20 * col) + offset;
	}
	else
	{
		y = 40 + 100 * row + offset;
	}
	leftClick(x, y);
}

void AvZ::shovelNotInQueue(int row, float col, bool pumpkin)
{
	safeClick();
	leftClick(50 * 13, 70);
	if (!pumpkin)
		clickGrid(row, col);
	else
		clickGrid(row, col, 10);
	safeClick();
}