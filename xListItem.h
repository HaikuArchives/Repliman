// xListItem.h: definition of an icon and text BListItem

#ifndef __xLISTITEM_H__
#define __xLISTITEM_H__

#include <ListItem.h>
#include <Bitmap.h>
#include <View.h>
#include <Bitmap.h>
#include <string.h>
#include <stdio.h>

class xListItem : public BListItem {
public:
					xListItem(const char* name, BBitmap *icon, int32 i, int32 location);

			int32	GetID() const { return fID; };
			int32	GetLocation() const { return fLocation; };
	const char*		GetName() const { return fName; };
protected:
	virtual void	DrawItem(BView *owner, BRect frame, bool complete = false);
	virtual void	Update(BView *owner, const BFont *font);
private:
	const char*		fName;
	BBitmap			*fIcon;
	int32			fID;
	int32			fLocation;
	float 			actual_height;
	int				text_pos;
	int				bitmap_pos;
};
#endif
// ----------------------------------------------------------- xListItem.h --