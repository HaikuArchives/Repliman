// xListItem.h: definition of an icon and text BListItem

#ifndef __xLISTHEADERITEM_H__
#define __xLISTHEADERITEM_H__

#include <ListItem.h>
#include <View.h>
#include <Bitmap.h>
#include <string.h>
#include <stdio.h>

class xHeaderListItem : public BListItem {
public:
					xHeaderListItem(const char* name, rgb_color background_colour);

protected:
	virtual void	DrawItem(BView *owner, BRect frame, bool complete = false);
	virtual void	Update(BView *owner, const BFont *font);
private:
	const char*		fName;
	float 			actual_height;
	int				text_pos;
	rgb_color		fColour;
	const BFont 	*current_font;
};
#endif
// ------------------------------------------------------- xHeaderListItem.h --