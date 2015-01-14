// xListItem.cpp: implementation of the xListItem class

#ifndef __xLISTITEM_H__
#include "xListItem.h"
#endif

#ifndef __COLOURS_H__
#include "Colours.h"
#endif

xListItem::xListItem(const char* name, BBitmap *icon, int32 i, int32 location)
	: BListItem()
{
	fName = strdup(name);
	fIcon = new BBitmap(icon);
	fID = i;
	fLocation = location;
}

void
xListItem::DrawItem(BView *owner, BRect frame, bool complete = false)
{
	rgb_color colour;

	if (IsSelected()) {
		colour = kHighlight;
	} else {
		colour = owner->ViewColor();
	}
			
	owner->SetLowColor(colour);
	owner->FillRect(frame, B_SOLID_LOW);

	owner->SetDrawingMode(B_OP_ALPHA);
	owner->DrawBitmap(fIcon, BPoint(frame.left + 4,frame.top + bitmap_pos));
	owner->SetDrawingMode(B_OP_COPY);

	owner->MovePenTo(frame.left + 28, frame.bottom - text_pos);


	if (IsEnabled())
		owner->SetHighColor(kBlack);
	else
		owner->SetHighColor(kMedGrey);

	owner->DrawString(fName);
}

void
xListItem::Update(BView *owner, const BFont *font)
{
	font_height height;
	
//	BListItem::Update(owner, font);
	
	font->GetHeight(&height);
	actual_height = ceil(height.ascent + height.descent);
	
	// set the new height for the menu item
	if (actual_height < 20)
		SetHeight(20);
	else
		SetHeight(actual_height);
		
	// calculate the positions of the bitmap and the text.
	text_pos = ceil(height.descent) + 2;
	bitmap_pos = (Height() - 16) / 2;
}
