// xButton.h : definition of a button class

#ifndef __xBUTTON_H__
#define __xBUTTON_H__

#include <Button.h>
#include <String.h>
#include <Application.h>
#include <Resources.h>
#include <Bitmap.h>
#include <stdio.h>

class xButton : public BButton
{
public:
					xButton(BRect rect, const char* name, const char* label, 
							BMessage *message);
	
	virtual void	ResizeToPreferred();
protected:
	virtual void	Draw(BRect updateRect);
	
	void			LoadBitmap();
private:
	BBitmap			*fIcon;
	float			fHeight;
};

#endif