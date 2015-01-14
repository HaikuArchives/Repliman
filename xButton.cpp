// xButton.cpp : implementation of the xButton class

#ifndef __xBUTTON_H__
#include "xButton.h"
#endif

xButton::xButton(BRect rect, const char* name, const char* label, BMessage *message)
	: BButton(rect, name, label, message)
{
	LoadBitmap();

	float space_width = StringWidth(" ");
	int num_spaces = floor(20.0 / space_width);
	BString new_button_str;
	
	for (int i=0; i < num_spaces; i++)
		new_button_str << " ";
		
	new_button_str << label;
	
	SetLabel(new_button_str.String());
}

void
xButton::Draw(BRect updateRect)
{
	BButton::Draw(updateRect);
	
	MovePenTo(8,fHeight);
	
	if (!IsEnabled())
		SetDrawingMode(B_OP_BLEND);
	else
		SetDrawingMode(B_OP_ALPHA);
	
	DrawBitmap(fIcon);
	SetDrawingMode(B_OP_COPY);
}

void
xButton::ResizeToPreferred()
{
	float width, height;
//	BButton::ResizeToPreferred();

	GetPreferredSize(&width, &height);
	if (height < 26.0)
		height = 26.0;
		
	ResizeTo(width, height);

	fHeight = height / 2.0 - 8.0;	
}

// if the Label() is used to generate the bitmap then the button should be created
// with the "english" name and then translated straight after.
void
xButton::LoadBitmap()
{
	BMessage *msg;
	size_t size;
	const void *data;
	
	BResources *res = be_app->AppResources();
	data = res->LoadResource('BBMP', Label(), &size);

	msg = new BMessage('vivi');
	msg->Unflatten((char *) data);
	
	fIcon = new BBitmap(msg);
}