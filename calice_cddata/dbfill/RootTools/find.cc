#include "find.hh"
#include <iostream>
#include <TROOT.h>
using namespace std;

TKey *find(TDirectory *working_dir, const TRegexp &reg, Int_t occurance, const char *class_name)
{
  //  RestoreDir restore;
  //  {
  Bool_t return_last_existing=kFALSE;
  if (occurance<=0) {
    occurance=1<<31;
    return_last_existing=kTRUE;
  }
  
  TList *list=working_dir->GetListOfKeys();
  TListIter iter(list);
  TKey *last=0;
  TKey *key;
  do {
    while ((key=(TKey *) iter.Next())) {
      TString name(key->GetName());
      if (name.Index(reg)>=0) {
	if (class_name) {
	  TClass *the_class=gROOT->GetClass(key->GetClassName());
	  if (the_class && the_class->GetBaseClass(class_name)) {
	    if (--occurance<=0) break;
	    if (key) {
	      last=key;
	    }
	  }
	} else  break;
      }
    }
  } while (key && --occurance>0);
  return (key ? key : (return_last_existing ? last : key));
  //}
}

TH1 *get_h1(TDirectory *working_dir,  const TRegexp &reg, Int_t occurance)
{
  TClass *a_class=TH1::Class();
  TKey *a_key=find(working_dir,reg,occurance,a_class);
  if (a_key) {
    TObject *object=a_key->ReadObj();
    if (object && object->InheritsFrom(a_class)) {
      return (TH1 *) object;
    }
    delete object;
  }
  return 0;
}

TDirectory *enter_dir(TDirectory *working_dir,  const TRegexp &reg, Int_t occurance)
{
  TClass *a_class=TDirectory::Class();
  TKey *a_key=find(working_dir,reg,occurance,a_class);
  if (a_key) {
    working_dir->cd(a_key->GetName());
    return gDirectory;
  }
  return 0;
}


TH2 *get_h2(TDirectory *working_dir,  const TRegexp &reg, Int_t occurance)
{
  TClass *a_class=TH2::Class();
  TKey *a_key=find(working_dir,reg,occurance,a_class);
  if (a_key) {
    TObject *object=a_key->ReadObj();
    if (object && object->InheritsFrom(a_class)) {
      return (TH2 *) object;
    }
    delete object;
  }
  return 0;
}

TGraph *get_graph(TDirectory *working_dir,  const TRegexp &reg, Int_t occurance)
{
  TClass *a_class=TGraph::Class();
  TKey *a_key=find(working_dir,reg,occurance,a_class);
  if (a_key) {
    TObject *object=a_key->ReadObj();
    if (object && object->InheritsFrom(a_class)) {
      return (TGraph *) object;
    }
    delete object;
  }
  return 0;
}

void list_files(TDirectory *working_dir, const TRegexp &reg, const char *class_name)
{
  TList *list=working_dir->GetListOfKeys();
  TListIter iter(list);
  TKey *key;
  do {
    while ((key=(TKey *) iter.Next())) {
      TString name(key->GetName());
      if (name.Index(reg)>=0) {
	if (class_name && strlen(class_name)>0) {
	  TClass *the_class=gROOT->GetClass(key->GetClassName());
	  if (!the_class || !the_class->GetBaseClass(class_name)) continue;
	}
	cout << name.Data() << ":" << key->GetClassName() << endl;
      }
    }
  } while (key);
}
