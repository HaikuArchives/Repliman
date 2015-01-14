// replimanWindow.cpp

#ifndef __REPLIMANWINDOW_H__
#include "replimanWindow.h"
#endif

#ifndef __xLISTITEM_H__
#include "xListItem.h"
#endif

#ifndef __xBUTTON_H__
#include "xButton.h"
#endif

#ifndef __xHEADERLISTITEM_H__
#include "xHeaderListItem.h"
#endif

#ifndef __COLOURS_H__
#include "Colours.h"
#endif

/*------------------------------------------------------------*/
struct match_info {
				match_info(image_id i, int32 loc) { id = i; location = loc; found = false; };
	image_id	id;
	int32		location;
	bool		found;
};

bool match_id(BListItem *item, void *data)
{
	match_info *mi = (match_info *) (data);

//	printf("match_id: %s\n", class_name(item));
	if (!(is_instance_of(item, xListItem))) {
//		printf("not an xListItem (%s)\n", class_name(item));
		return false;
	}

	xListItem *my = dynamic_cast<xListItem*>(item);
	if (my->GetID() == (mi)->id && my->GetLocation() == (mi)->location) {
		(mi)->found = true;
		return true;
	}
	return false;
}

/*------------------------------------------------------------*/
/*------------------------------------------------------------*/
/*------------------------------------------------------------*/

replimanWindow::replimanWindow()
	: BWindow(BRect(100, 50, 380, 300), "repliman", B_TITLED_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE),
		fTickToken(BMessenger(this), new BMessage(CMD_TICK), 1000000)
{
	InitGUI();
	
	UpdateList(true);
}

/*------------------------------------------------------------*/
replimanWindow::~replimanWindow()
{
}

/*------------------------------------------------------------*/

bool
replimanWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return TRUE;
}

#pragma mark -

void
replimanWindow::InitGUI()
{
	BRect b = Bounds();
	BScrollView *sv;
	
	BView *parent = new BView(b, "parent", B_FOLLOW_ALL, B_WILL_DRAW);
	parent->SetViewColor(216, 216, 216, 0);
	AddChild(parent);

	BRect	r = b;
	
	r.InsetBy(10,10);
	r.right = b.right - 10 - B_H_SCROLL_BAR_HEIGHT;
	r.bottom = r.top + (20 * 4) + 3;
	
	fReplicantList = new BListView(r, "rep_list", B_SINGLE_SELECTION_LIST, 
		B_FOLLOW_NONE);
	fReplicantList->SetSelectionMessage(new BMessage(CMD_REP_SELECTION_CHANGED));
//	fReplicantList->AddItem(new xHeaderListItem("Deskbar", kBlack));
	
	// add the scroll view
	sv = new BScrollView("rep_scroll_view", fReplicantList, B_FOLLOW_NONE,
		B_WILL_DRAW, false, true);

	parent->AddChild(sv);
	// and resize the scroll bar - after AddChild
//	BScrollBar *scroll_bar = sv->ScrollBar(B_VERTICAL);
//	scroll_bar->SetSteps(19, 80);
	
	r.top = r.bottom + 10;
	
	fDeleteButton = new xButton(r, "del", "Delete",
		new BMessage(CMD_DELETE_REPLICANT));
	fDeleteButton->ResizeToPreferred();
	fDeleteButton->SetEnabled(false);
	parent->AddChild(fDeleteButton);
	
	ResizeTo(b.right, r.bottom + 20 + fDeleteButton->Bounds().bottom);
}
/*------------------------------------------------------------*/

void
replimanWindow::MessageReceived(BMessage *msg)
{		
	switch (msg->what) {
		case CMD_TICK: {
			UpdateList(false);
			break;
		}
		case CMD_DELETE_REPLICANT: {
			int32	sel = fReplicantList->CurrentSelection();
			xListItem *item = dynamic_cast<xListItem*>(fReplicantList->ItemAt(sel));
			ASSERT(sel >= 0);
			ASSERT(item);
//			printf("deleting %s with id = %ld\n", item->GetName(), item->GetID());
//			printf("after delete %s\n", strerror(DeleteReplicant(item->GetID(), item->GetLocation())));
			DeleteReplicant(item->GetID(), item->GetLocation());
			break;
		}
		case CMD_REP_SELECTION_CHANGED: {
			bool	enabled;
			enabled = (fReplicantList->CurrentSelection() >= 0);
			fDeleteButton->SetEnabled(enabled);
			break;
		}
		default:
			BWindow::MessageReceived(msg);
	}
}

/*------------------------------------------------------------*/
#pragma mark -

void
replimanWindow::EmptyLists()
{
	fReplicantList->MakeEmpty();
	fHaveDesktopHeader = fHaveDeskbarHeader = false;
	fDesktopCount = fDeskbarCount = 0;
}

void
replimanWindow::UpdateList(bool make_empty)
{
	bool result;
	
	if (make_empty)
		EmptyLists();
		
	result = UpdateQuasiList(M_DESKTOP, make_empty);
	result = UpdateQuasiList(M_DESKBAR, make_empty) || result;
	
	if (result || make_empty)
		PostMessage(CMD_REP_SELECTION_CHANGED);
}

// 2004/10/10 - I would think that it is better if this
// function handled better the case in which the tracker
// or deskbar is not loaded
bool
replimanWindow::UpdateQuasiList(int32 target, bool make_empty)
{
	bool	deleted_something = false;
	int32	index = 0;
	int32	uid;
	
	// first we set the target
	fTarget = MessengerForTarget(target);

	// if the target does not exist then remove the items that were there
	if (!fTarget.IsValid()) {
//		printf("target is not valid\n");
		int32 count, pos;
		
		if (target == M_DESKTOP) {
			count = (fHaveDesktopHeader ? fDesktopCount+1 : 0);
			pos = 0;
			fHaveDesktopHeader = false;
			fDesktopCount = 0;
		} else {
			count = (fHaveDeskbarHeader ? fDeskbarCount+1 : 0);
			pos = (fHaveDesktopHeader ? fDesktopCount+1 : fDesktopCount);
			fHaveDeskbarHeader = false;
			fDeskbarCount = 0;
		}
		
		for (int i=0; i < count; i++)
			fReplicantList->RemoveItem(pos);
			
		deleted_something = true;

		return deleted_something;
	}
	
	if (!make_empty) {
		// walk through the current list of replicants and remove any that are
		// no longer loaded
		xListItem 	*item;
		int32 start, count;
		
		if (target == M_DESKTOP) {
			start = 1;
			count = fDesktopCount;
		} else {
			start = (fHaveDesktopHeader ? fDesktopCount+1 : 0);
			
			if (fHaveDeskbarHeader)
				start++;
				
			count = fDeskbarCount;
		}
		
//		printf("start = %ld, count = %ld\n", start, count);
		for (int i=start; i < start+count; i++) {
			item = dynamic_cast<xListItem*>(fReplicantList->ItemAt(i));
			int32 uid = item->GetID();
			if (IsReplicantLoaded(uid) == false) {
				fReplicantList->RemoveItem(item);
				delete item;
				deleted_something = true;
				if (target == M_DESKTOP)
					fDesktopCount--;
				else
					fDeskbarCount--;
				count--;
//				printf("x");
			} //else
//				printf(".");
		}
		
		// if there are no items but still the header delete it
		if (target == M_DESKTOP) {
			if (fDesktopCount == 0 && fHaveDesktopHeader) {
				fReplicantList->RemoveItem((int32)0);
				fHaveDesktopHeader = false;
			}
		} else {
			if (fDeskbarCount == 0 && fHaveDeskbarHeader) {
				int32 pos = (fHaveDesktopHeader ? fDesktopCount + 1 : 0);
				fReplicantList->RemoveItem(pos);
				fHaveDeskbarHeader = false;
			}
		}
	}
	
	// Now get all the replicants from the shelf and make sure that they are in
	// the list
	while ((uid = GetReplicantAt(index++)) >= B_OK) {
		// if this uid is already in the list then skip it
		match_info	mi(uid, target);
		fReplicantList->DoForEach(match_id, &mi);
		if (mi.found) {
			continue;
		}
		
		BMessage	rep_info;
		if (GetReplicantName(uid, &rep_info) != B_OK) {
			continue;
		}
		const char *name;
		if (rep_info.FindString("result", &name) == B_OK) {
			int32 pos;
			BBitmap *icon;
			icon = GetReplicantIcon(uid);

			if (target == M_DESKTOP) {
				pos = (fHaveDesktopHeader ? fDesktopCount+1 : 0);			
			} else {
				pos = (fHaveDesktopHeader ? fDesktopCount+1 : 0);			
				pos += (fHaveDeskbarHeader ? fDeskbarCount+1 : 0);			
			}
			
			fReplicantList->AddItem(new xListItem(name, icon, uid, target), pos);
			if (target == M_DESKTOP)
				fDesktopCount++;
			else
				fDeskbarCount++;
		}
	}

	// we should add the header if it does not exist
	if (target == M_DESKTOP) {
		if (!fHaveDesktopHeader && fDesktopCount != 0) {
			fReplicantList->AddItem(new xHeaderListItem("Desktop", kFirstGreen),0);
			fHaveDesktopHeader = true;
		}
	} else {
		if (!fHaveDeskbarHeader && fDeskbarCount != 0) {
			int32 pos = (fHaveDesktopHeader ? fDesktopCount + 1 : 0);
			fReplicantList->AddItem(new xHeaderListItem("Deskbar", kSecondGreen),pos);
			fHaveDeskbarHeader = true;
		}
	}
	
	return deleted_something;
}

/*------------------------------------------------------------*/
#pragma mark -

status_t
replimanWindow::GetReplicantName(int32 uid, BMessage *reply) const
{
	/*
	 We send a message to the target shelf, asking it for the Name of the 
	 replicant with the given unique id.
	 */
	 
	BMessage	request(B_GET_PROPERTY);
	BMessage	uid_specifier(B_ID_SPECIFIER);	// specifying via ID
	status_t	err;
	status_t	e;
	
	request.AddSpecifier("Name");		// ask for the Name of the replicant
	
	// IDs are specified using code like the following 3 lines:
	uid_specifier.AddInt32("id", uid);
	uid_specifier.AddString("property", "Replicant");
	request.AddSpecifier(&uid_specifier);
	
	if ((err = fTarget.SendMessage(&request, reply)) != B_OK)
		return err;
	
	if (((err = reply->FindInt32("error", &e)) != B_OK) || (e != B_OK))
		return err ? err : e;
	
	return B_OK;
}

BBitmap *
replimanWindow::GetReplicantIcon(int32 uid)
{
	BMessage	request(B_GET_PROPERTY);		// Get will return the archive msg
	BMessage	uid_specifier(B_ID_SPECIFIER);	// specifying via ID
	BMessage	reply;
	status_t	err;
	status_t	e;
			
	// IDs are specified using code like the following 3 lines:
	uid_specifier.AddInt32("id", uid);
	uid_specifier.AddString("property", "Replicant");
	request.AddSpecifier(&uid_specifier);
	
	if ((err = fTarget.SendMessage(&request, &reply)) != B_OK)
		return NULL;
	
	if (((err = reply.FindInt32("error", &e)) != B_OK) || (e != B_OK))
		return NULL;
	
	// OK, let's get the archive message
	BMessage	data;
	reply.FindMessage("result", &data);

	BBitmap *icon = new BBitmap(BRect(0,0,15,15), B_CMAP8);
	// what is the signature of this class?
	const char *name;
	if (data.FindString("add_on", &name) == B_OK) {
		entry_ref ref;
		BMimeType mimetype(name);
		mimetype.GetAppHint(&ref);
//		printf("add_on = %s and %s\n", name, appInfo.ref.name);
//		BNode node(&appInfo.ref);
//		BNodeInfo nodeinfo(&node);
		status_t status = BNodeInfo::GetTrackerIcon(&ref, icon, B_MINI_ICON);

		if (status != B_OK) {
//			printf("problem getting icon %s\n", strerror(status));
			// so let us load the zombie icon!
			BMessage *msg;
			size_t size;
			const void *data;
	
			BResources *res = be_app->AppResources();
			data = res->LoadResource('BBMP', "Zombie"/*Label()*/, &size);

			msg = new BMessage('vivi');
			msg->Unflatten((char *) data);
			
			delete icon;
			icon = new BBitmap(msg);
		}
	} //else { // we do not know the app_signature or icon...

	return icon;
}

/*------------------------------------------------------------*/

bool
replimanWindow::IsReplicantLoaded(int32 uid) const
{
	/*
	 determine if the specified replicant (the unique ID of the replicant)
	 still exists in the target container/shelf. If we can get the name then the 
	 replicant still exists.
	 */
	BMessage	reply;	
	status_t	err = GetReplicantName(uid, &reply);
	return (err == B_OK);
}

/*------------------------------------------------------------*/

int32
replimanWindow::GetReplicantAt(int32 index) const
{
	/*
	 So here we want to get the Unique ID of the replicant at the given index
	 in the target Shelf.
	 */
	 
	BMessage	request(B_GET_PROPERTY);		// We're getting the ID property
	BMessage	reply;
	status_t	err;
	
	request.AddSpecifier("ID");					// want the ID
	request.AddSpecifier("Replicant", index);	// of the index'th replicant
	
	if ((err = fTarget.SendMessage(&request, &reply)) != B_OK)
		return err;
	
	int32	uid;
	if ((err = reply.FindInt32("result", &uid)) != B_OK) 
		return err;
	
	return uid; 
}

/*------------------------------------------------------------*/

BMessenger
replimanWindow::MessengerForTarget(type_code w) const
{
	/*
	 This function determines the BMessenger to the various Shelf objects
	 that this app can talk with.
	 */
	BMessage	request(B_GET_PROPERTY);
	BMessenger	to;
	BMessenger	result;

	request.AddSpecifier("Messenger");
	request.AddSpecifier("Shelf");
	
	switch (w) {
		case M_SET_DESKBAR_TARGET: {
			// In the Deskbar the Shelf is in the View "Status" in Window "Deskbar"
			request.AddSpecifier("View", "Status");
			request.AddSpecifier("Window", "Deskbar");
			to = BMessenger("application/x-vnd.Be-TSKB", -1);
			break;
		}
		case M_SET_DESKTOP_TARGET: {
			// The Desktop is owned by Tracker and the Shelf is in the
			// View "PoseView" in Window "Desktop"
			request.AddSpecifier("View", "PoseView");
			request.AddSpecifier("Window", "/boot/home/Desktop");
			to = BMessenger("application/x-vnd.Be-TRAK", -1);
			break;
		}
	}
	
	BMessage	reply;
	
	if (to.SendMessage(&request, &reply) == B_OK) {
		reply.FindMessenger("result", &result);
	}
	return result;
}

/*------------------------------------------------------------*/

status_t
replimanWindow::DeleteReplicant(int32 uid, int32 location)
{
	status_t status;
	
//	printf("deleting replicant\n");
	// delete the given replicant from the deskbar
	if (location == M_DESKBAR) {
		BDeskbar desk;
		status = desk.RemoveItem(uid);
	} else {
		BMessage	request(B_DELETE_PROPERTY);		// Delete
		BMessage	uid_specifier(B_ID_SPECIFIER);	// specifying via ID
		BMessage	reply;
		status_t	err;
		status_t	status;

		fTarget = MessengerForTarget(M_SET_DESKTOP_TARGET);

		// specify the id of the replicant
		uid_specifier.AddInt32("id", uid);
		uid_specifier.AddString("property", "Replicant");
		request.AddSpecifier(&uid_specifier);
	
		if ((err = fTarget.SendMessage(&request, &reply)) != B_OK)
			return err;
	
		if ((err = reply.FindInt32("error", &status)) != B_OK)
			return err;
	
	}

	return status;
}