/*
	Copyright 1999, Be Incorporated.   All Rights Reserved.
	This file may be used under the terms of the Be Sample Code License.
*/

#ifndef __REPLIMANWINDOW_H__
#define __REPLIMANWINDOW_H__

#include <Application.h>
#include <Messenger.h>
#include <Window.h>
#include <Shelf.h>
#include <ListView.h>
#include <MessageRunner.h>
#include <Button.h>
#include <Dragger.h>
#include <Box.h>
#include <ScrollView.h>
#include <MenuItem.h>
#include <ListItem.h>
#include <StringView.h>
#include <image.h>
#include <Debug.h>
#include <Mime.h>
#include <Node.h>
#include <NodeInfo.h>
#include <String.h>
#include <Deskbar.h>
#include <ClassInfo.h>

/*------------------------------------------------------------*/

enum {
	M_SET_DESKBAR_TARGET		= 'sdes',
	M_SET_DESKTOP_TARGET		= 'strk',
	CMD_DELETE_REPLICANT		= 'delr',
	CMD_REP_SELECTION_CHANGED	= 'rslc',
	CMD_TICK					= 'tick',
	
	M_DESKBAR					= M_SET_DESKBAR_TARGET,
	M_DESKTOP					= M_SET_DESKTOP_TARGET
};
	
/*------------------------------------------------------------*/
class xButton;

class replimanWindow : public BWindow {
public:
					replimanWindow();
					~replimanWindow();
protected:
	virtual void	MessageReceived(BMessage *msg);
	virtual bool	QuitRequested();

private:
	void			InitGUI();

	// These are the interesting functions showing how you can 
	// interact via BMessages with a Shelf/Container view
	BMessenger		MessengerForTarget(type_code w) const;
	int32			GetReplicantAt(int32 index) const;
	status_t		GetReplicantName(int32 uid, BMessage *result) const;
	BBitmap	*		GetReplicantIcon(int32 uid);
	status_t		DeleteReplicant(int32 uid, int32 location);
	
	bool			IsReplicantLoaded(int32 uid) const;
		
	void			UpdateList(bool make_empty);
	bool			UpdateQuasiList(int32 target, bool make_empty);
	void			EmptyLists();
		
	BListView		*fReplicantList;
	int				fDesktopCount;
	int				fDeskbarCount;
	bool			fHaveDesktopHeader;
	bool			fHaveDeskbarHeader;
	BMessenger		fTarget;
	BMessageRunner	fTickToken;
	xButton			*fDeleteButton;
};
#endif