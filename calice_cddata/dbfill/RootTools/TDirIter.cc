#include <TDirectory.h>
#include <TKey.h>
#include <TList.h>
#include <TClass.h>
#include <string.h>
#include <iostream>
#include <TRegexp.h>
#include <TString.h>
#include <TROOT.h>
#include "TDirIter.hh"

using namespace std;


Bool_t TDirIter::IsRegexp(const char *name)
{
  const char *ptr=name;
  while (*ptr) {
    switch (*ptr) {
    case '[':
    case '(':
    case ']':
    case ')':
    case '*':
      return kTRUE;
    }
    ptr++;
  }
  return kFALSE;
}


TRegexp TDirIter::MakeRegexp(const char *name_or_regexp)
{
  if (!name_or_regexp || strlen(name_or_regexp)==0) return TRegexp("^");
  
  if (IsRegexp(name_or_regexp)) {
    return TRegexp(name_or_regexp);
  }
  else {
    return TRegexp(TString("^")+name_or_regexp+"$");
  }
  return TRegexp("^");
}

TDirIter::TDirIter(TDirectory *a_dir,const char *name_or_regexp, const char *class_name)
  :  fDir(a_dir),
     fClassName(class_name),
     fIter(0),
     fKey(0),
     fRegexp(MakeRegexp(name_or_regexp))
{
  Init();
}

TDirIter::TDirIter(TDirectory *a_dir,const TRegexp &reg, const char *class_name)
:  fDir(a_dir),
   fClassName(class_name),
   fIter(0),
   fKey(0),
   fRegexp(reg)
    
{
  Init();
}

TDirIter::~TDirIter()
{
}


void TDirIter::Init()
{
  if (fDir) {
    TList *list=fDir->GetListOfKeys();
    fIter=TListIter(list);
  }
}

void TDirIter::Reset()
{
  fIter.Reset();
}

TKey *TDirIter::operator++(int) //postfix
{
  TKey *old_key=fKey;
  Next();
  return old_key;
}

TKey *TDirIter::Next() //prefix
{
  //  RestoreDir restore;
  //  {
  TKey *key;
  while ((key=(TKey *) fIter.Next())) {
    //    cout << key->GetName()  << "(" << key->GetClassName() << ")" << endl;
    TString name(key->GetName());
    if (name.Index(fRegexp)>=0) {
      if (fClassName) {
	TClass *the_class=gROOT->GetClass(key->GetClassName());
	if (the_class && the_class->GetBaseClass(fClassName)) {
	  break;
	}
      } else  break;
    }
  }
  return fKey=key;
  //}
}
