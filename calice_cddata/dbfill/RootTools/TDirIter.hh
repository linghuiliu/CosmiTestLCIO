//#ifndef _TDirIter_H_
//#define _TDirIter_H_

#include <TKey.h>
#include <TList.h>
#include <TDirectory.h>
#include <TRegexp.h>
#include <TString.h>

class TDirIter
{
public:
  TDirIter(TDirectory *a_dir,const char *name_or_regexp, const char *class_name);
  TDirIter(TDirectory *a_dir,const TRegexp &reg, const char *class_name);
  TDirIter(const  TDirIter &a) : fDir(0),fClassName(a.fClassName),fIter(0),fRegexp(a.fRegexp) {Init();};

  static TRegexp MakeRegexp(const char *name_or_regexp);
  static Bool_t IsRegexp(const char *name);

  virtual ~TDirIter();
  virtual void Init();
  virtual void Reset();
  virtual TKey *operator++(int); //postfix;
  virtual TKey *Next(); //prefix;
  TKey *GetLastKey() {return fKey;};
  const char *GetDirName() {return fDir->GetName();};

protected:
  TDirectory *fDir;
  TString fClassName;
  TListIter fIter;
  TKey *fKey;
  TRegexp fRegexp;
};

//#endif

