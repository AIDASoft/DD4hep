// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// Framework include files
#include "DDEve/ContextMenu.h"

// ROOT include files
#include "TList.h"
#include "TClassMenuItem.h"

// C/C++ include files
#include <stdexcept>
#include <map>

using namespace std;
using namespace DD4hep;

typedef map<string,ContextMenu*> Contexts;
static Contexts& mapped_entries()  {
  static Contexts e;
  return e;
}

ClassImp(ContextMenuHandler)

/// Initializing constructor
ContextMenuHandler::ContextMenuHandler(Callback cb, void* par)
: m_call(cb), m_param(par) 
{
}

/// Default destructor
ContextMenuHandler::~ContextMenuHandler()   {
}

/// Callback
void ContextMenuHandler::Context(TObject* target)   {
  const void *args[] = {target,m_param,0};
  m_call.execute(args);
}


ClassImp(ContextMenu)

/// Initializing constructor
ContextMenu::ContextMenu(TClass* cl) : m_class(cl)  {
  if ( !cl )   {
    throw runtime_error("Failure: Cannot create context menu for NULL class!");
  }
}

/// Default destructor
ContextMenu::~ContextMenu()   {
}

/// Instantiator
ContextMenu& ContextMenu::instance(TClass* cl)  {
  Contexts::const_iterator i = mapped_entries().find(cl->GetName());
  if ( i != mapped_entries().end() ) return *((*i).second);
  ContextMenu* m = new ContextMenu(cl);
  mapped_entries().insert(make_pair(cl->GetName(),m));
  return *m;
}

/// Clear all existing items
ContextMenu& ContextMenu::Clear()   {
  if ( m_class->GetMenuList() )  {
    m_class->GetMenuList()->Delete();
  }
  return *this;
}

/// Add a separator
ContextMenu& ContextMenu::AddSeparator()   {
  TClassMenuItem* item = 
    new TClassMenuItem(TClassMenuItem::kPopupSeparator,ContextMenuHandler::Class(),"seperator");
  m_class->GetMenuList()->AddFirst(item);
  return *this;
}

/// Add user callback 
ContextMenu& ContextMenu::Add(const string& title, Callback cb, void* ud)   {
  ContextMenuHandler* handler = new ContextMenuHandler(cb, ud);
  TClassMenuItem* item = 
    new TClassMenuItem(TClassMenuItem::kPopupUserFunction,
		       ContextMenuHandler::Class(),title.c_str(),
		       "Context",handler,"TObject*",2);
  m_calls.push_back(handler);
  m_class->GetMenuList()->AddLast(item);
  return *this;
}
