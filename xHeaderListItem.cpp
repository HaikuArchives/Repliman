// xHeaderListItem.cpp: implementation of the xHeaderListItem class

#ifndef __xHEADERLISTITEM_H__
#include "xHeaderListItem.h"
#endif

#ifndef __COLOURS_H__
#include "Colours.h"
#endif

xHeaderListItem::xHeaderListItem(const char* name, rgb_color background_colour)
	: BListItem(), fColour(background_colour)
{
	fName = strdup(name);
	SetEnabled(false);
}

void
xHeaderListItem::DrawItem(BView *owner, BRect frame, bool complete = false)
{
	owner->SetLowColor(fColour);
	owner->FillRect(frame, B_SOLID_LOW);

	owner->SetHighColor(kWhite);

	owner->MovePenTo(frame.left + 4, frame.bottom - text_pos);
//	owner->SetFont(be_bold_font);
	owner->DrawString(fName);
}

void
xHeaderListItem::Update(BView *owner, const BFont *font)
{
	font_height height;
	
	//BListItem::Update(owner, be_bold_font);

	font->GetHeight(&height);
	actual_height = ceil(height.ascent + height.descent + height.leading);
	
//	printf("heights are: ascent: %.2f, descent: %.2f, leading: %.2f\n",
	//		height.ascent, height.descent, height.leading);
	SetHeight(actual_height+2);
		
	// calculate the positions of the bitmap and the text.
	text_pos = ceil(height.descent);//(Height() - actual_height) / 2;
}
