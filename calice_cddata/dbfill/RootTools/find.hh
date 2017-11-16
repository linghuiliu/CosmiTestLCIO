#ifndef _Find_H_
#define _Find_H_

#if (!defined(__CINT__) || defined(__MAKECINT__))
#include <TFile.h>
#include <TDirectory.h>
#include <TKey.h>
#include <TList.h>
#include <TRegexp.h>
#include <TClass.h>

#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#endif


TKey *find(TDirectory *working_dir, const TRegexp &reg, Int_t occurance, const char *class_name);

inline TKey *find(TDirectory *working_dir, const TRegexp &reg, Int_t occurance, TClass *a_class)
{
  return find(working_dir,reg,occurance,a_class->GetName());
}

void list_files(TDirectory *a_dir, const TRegexp &reg, const char *class_name=0);

TH1 *get_h1(TDirectory *working_dir,  const TRegexp &reg, Int_t occurance);
TDirectory *enter_dir(TDirectory *working_dir,  const TRegexp &reg, Int_t occurance);
TH2 *get_h2(TDirectory *working_dir,  const TRegexp &reg, Int_t occurance);
TGraph *get_graph(TDirectory *working_dir,  const TRegexp &reg, Int_t occurance);


template <class T> T *get_obj(TKey *a_key) 
{
  TObject *object=a_key->ReadObj();
  if (object && object->InheritsFrom(T::Class())) {
    return static_cast<T *>(object);
  }
  delete object;
  return 0;
}


template <class T> T *get(TDirectory *working_dir,  const TRegexp &reg, Int_t occurance)
{
  TClass *a_class=T::Class();
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
#endif
