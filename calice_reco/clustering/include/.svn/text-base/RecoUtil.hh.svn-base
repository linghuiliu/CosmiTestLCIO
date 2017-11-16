#ifndef RECOUTIL_HH
#define RECOUTIL_HH

// Data storage and manipulation functions
// for events reconstruction
// Alexey Zhelezov, DESY/ITEP 2006
//(previous name: reco_struct.hh)
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

//using namespace std;

//
// My container.
//  General idea - fast explicit container which can allocate
//                 arrays and never reallocate anything.
//                 No navigation requirements.
//                 Must be easy deallocatable

//===================================================================
/** This class is to provide some base class
    for all templated implementations.
    */
class SpacePool {
  private:
    SpacePool *next;
  protected:
    unsigned count;     /**<common data in all holders*/
    unsigned alloced;
    unsigned _prealloc;
  
    void connect_to(SpacePool &x){
      next   = x.next;
      x.next = this;
    }
    
    SpacePool(SpacePool &x, bool) : 
      count(x.count), alloced(x.alloced), _prealloc(x._prealloc) {
      x.count = x.alloced = 0;
      connect_to(x);
    }

  public:
    /**Constructor*/
    SpacePool(unsigned prealloc=256) : 
      next(0), count(0), alloced(0), _prealloc(prealloc) { }

    /**Constructor*/
    SpacePool(SpacePool &x, unsigned prealloc=256) : 
      count(0), alloced(0), _prealloc(prealloc) {
      connect_to(x); 
    }

    /**Destructor*/
    virtual ~SpacePool() { delete next; }
};

/** Full version.
    Items must have ctor and dtor
    This version can be used with any class, but
    can be significantly slower then FastSpaceHolder
    */
template <class _Tp> class SpaceHolder : public SpacePool {
    _Tp *data;
  
    SpaceHolder(SpaceHolder<_Tp> &x,bool) : 
      SpacePool(x,true), data(x.data) {
      x.data = 0;
    }

  public:
    /**Constructor */
    SpaceHolder(SpacePool &pool, unsigned prealloc=256) : 
      SpacePool(pool,prealloc), data(0) {}

    /**Destructor */
    virtual ~SpaceHolder(){
      for(unsigned i = 0; i < count; i++)
	data[i].~_Tp(); // call dtor for objects, for which ctor was called
      operator delete(data);
    }
    /** Create array of type _Tp
	@param n number of elements in array ?
	*/
    _Tp *array(unsigned n)
    {
      if(count + n > alloced) {
	if(alloced)
	  new SpaceHolder<_Tp>(*this, true);
	alloced = n + _prealloc;
	void *p = operator new(alloced*sizeof(_Tp));
	memset(p, 0, alloced*sizeof(_Tp));
	data = (_Tp *)p;
      }

      for(unsigned i = 0; i < n; i++)
	new (data + count + i) _Tp();   // call ctor for allocated object
      count += n;
      return data + count - n;
    }
 
    /** Allocate space for _Tp
	*/
    _Tp *alloc() { return array(1); }
};


template <class _Tp> _Tp *sh_array(SpacePool &pool,unsigned n);


/** Fast version, but:
    it is impossible put into this container
    something with implicit or explicit
    ctor/dtor (no virtual functions).
    */
template <class _Tp> class FastSpaceHolder : public SpacePool {
      _Tp *data;
  
      FastSpaceHolder(FastSpaceHolder<_Tp> &x, bool) : 
	SpacePool(x, true), data(x.data) {
	x.data = 0;
      }

    public:
      FastSpaceHolder(SpacePool &pool, unsigned prealloc=256) : 
	SpacePool(pool,prealloc), data(0) {}

    /**Destructor */
      virtual ~FastSpaceHolder() { 
	operator delete(data); }

    /** Create array of type _Tp 
	@param n size of array
	*/
      _Tp *array(unsigned n){
	if(count + n > alloced) {
	  if(alloced)
	    new FastSpaceHolder<_Tp>(*this, true);
	  alloced = n + _prealloc;
	  void *p = operator new(alloced * sizeof(_Tp));
	  memset(p, 0, alloced*sizeof(_Tp));
	  data = (_Tp *)p;
	}
	count += n;
	return data + count - n;
      }
    /** Allocate space for _Tp
	*/
      _Tp *alloc() { 
	return array(1); 
      }
  };


template <class _Tp> _Tp *fsh_array(SpacePool &pool,unsigned n);

//
// Reco Structure
//

typedef unsigned RSType;
typedef unsigned RSTypeMask;

#define RSMakeTypeMask(t) (1<<(t))

class RS;
class RSObj;
class RSLink;
class RSLinkGroup;
class RSIter;
template <class _Tp> class RSIterator;

template <class _Tp_self, class _Tp_prt>
  class RSSList {
    public:
      _Tp_prt *prt;
      _Tp_self *nxt;
  };

template <class _Tp_self,class _Tp_prt>
  class RSList : public RSSList<_Tp_self,_Tp_prt> {
    public:
      _Tp_self *prv;
  };

class RSLink : public RSList<RSLink,RSLinkGroup> {
  public:
    RSObj  *obj;
    RSLink *nxt_lnk;
};

class RSLinkGroup : public RSSList<RSLinkGroup,RSObj> {
  public:
    RSLink *cld;
    RSLink *lcld;

    RSTypeMask rs_type_mask;
    unsigned size;

    bool HasA(RSType type); 
    void init(RSObj *_prt,RSTypeMask tmask);
    void prepend(RSLink *lnk);
    void append(RSLink *lnk);
    void remove(RSLink *lnk);
};

//===============================================================================
/** Class for ???
    */
class RSObj {
  public:
    // RTTI
    //  static const RSType     RS_TYPE;  // Must be defined for all RS objected
    RSTypeMask rs_type_mask;

    /** Check if the class is a real RSObj
	*/
    template <class _Tp> bool IsA() { 
      return (rs_type_mask & (1<<_Tp::RS_TYPE))?true:false; 
    }

    // Tree operations
    RSLinkGroup *cld;
    RSLink      *prt;

    template <class _Tp> unsigned getNumberOf() {
      RSLinkGroup *lg = cld;
      unsigned size = 0;

      while(lg){
	if(lg->HasA(_Tp::RS_TYPE))
	  size += lg->size;
	lg = lg->nxt;
      }
      return size;
    }

    template <class _Tp> _Tp* getChild() {
      RSLinkGroup *lg = cld;

      while(lg){
	if(lg->HasA(_Tp::RS_TYPE))
	  return static_cast<_Tp *>(lg->cld->obj);
	lg = lg->nxt;
      }
      return 0;
    }

    template <class _Tp> unsigned getNumberOfParents() {
      unsigned size=0;
      RSLink *lnk=prt;
      while(lnk){
	RSLinkGroup *lg=lnk->prt;
	RSObj *obj=lg->prt;
	if(obj->IsA(_Tp::RS_TYPE))
	  size++;
	lnk=lnk->nxt_lnk;
      }
      return size;
    }

    template <class _Tp> _Tp* getParent() {
      RSLink *lnk=prt;
      while(lnk){
	RSLinkGroup *lg=lnk->prt;
	RSObj *obj=lg->prt;
	if(obj->IsA(_Tp::RS_TYPE))
	  return static_cast<_Tp *>(obj);
	lnk=lnk->nxt_lnk;
      }
      return 0;
    }

    // private:
    RSLink *getPrtLink(RSObj *_prt);
    void    remove(RSLinkGroup *lg);
    // Not so nice, but can't write templated functions otherwise...
    unsigned getNumberOf(RSType type);
    RSObj* getParent(RSType type);
    bool IsA(RSType type) {
      return (rs_type_mask & (1<<type))?true:false; 
    }

    // other...

    virtual ~RSObj() {}

  protected:
    RSObj(RSType type) {
      rs_type_mask=1<<type;
      cld=0;
      prt=0;
    }

    void AddRSType(RSType type){
      rs_type_mask|=(1<<type);
    }

};


/** Something
    */
class RS {
  private:
    FastSpaceHolder<RSLink> *link_pool;
    FastSpaceHolder<RSLinkGroup> *link_group_pool;


  public:
  
    void *obj_pool[32]; // !!! must be private, but I need rs_alloc


    SpacePool pool;

    RS() {
      link_pool=new FastSpaceHolder<RSLink>(pool);
      link_group_pool=new FastSpaceHolder<RSLinkGroup>(pool);
      memset(obj_pool,0,sizeof(obj_pool));
    }

    template<class _Tp>
      _Tp *alloc(){
      RSType type=_Tp::RS_TYPE;
      if(type>31)
	return 0;
      if(!obj_pool[type])
	obj_pool[type]=new SpaceHolder<_Tp>(pool);
      return (static_cast<SpaceHolder<_Tp>*>(obj_pool[type]))->alloc();
    }

    void unlink(RSObj *cld,RSObj *prt){
      RSLink *lnk=cld->getPrtLink(prt);
      if(!lnk)
	return;
      RSLinkGroup *lg=lnk->prt;
      lg->remove(lnk);
      if(!lg->size)
	prt->remove(lg);
      // will be nice to reuse lnk and lg afterwars...
    }

    void unlink(RSObj *obj){
      // unlink form all parents
      while(obj->prt){
	RSObj *prt=obj->prt->prt->prt;
	unlink(obj,prt);
      }
      // unlink all children
      while(obj->cld){
	RSObj *cld=obj->cld->cld->obj;
	unlink(cld,obj);
      }
    }

    void prepend(RSObj *cld,RSObj *prt){
      unlink(cld,prt);
      RSLink *lnk=link_pool->alloc();
      RSLinkGroup *lg=prt->cld;
      while(lg){
	if(lg->rs_type_mask==cld->rs_type_mask)
	  break;
	lg=lg->nxt;
      }
      if(!lg){
	lg=link_group_pool->alloc();
	lg->init(prt,cld->rs_type_mask);
      }
      lnk->obj=cld;
      lg->prepend(lnk);
      lnk->nxt_lnk=cld->prt;
      cld->prt=lnk;
    }

    void append(RSObj *cld,RSObj *prt){
      unlink(cld,prt);
      RSLink *lnk=link_pool->alloc();
      RSLinkGroup *lg=prt->cld;
      while(lg){
	if(lg->rs_type_mask==cld->rs_type_mask)
	  break;
	lg=lg->nxt;
      }
      if(!lg){
	lg=link_group_pool->alloc();
	lg->init(prt,cld->rs_type_mask);
      }
      lnk->obj=cld;
      lg->append(lnk);
      lnk->nxt_lnk=cld->prt;
      cld->prt=lnk;
    }

    template <class _Tp>
      void SortChildren(RSObj *obj,int (*compare)(const _Tp **,const _Tp **)){
      unsigned count=obj->getNumberOf(_Tp::RS_TYPE);
      if(!count)
	return;
      static void **ar=0;
      static unsigned ar_size=0;
      if(ar_size<count){
	if(ar_size)
	  delete[] ar;
	ar_size=count;
	ar=new void *[ar_size];
      }
      RSIterator<_Tp> it(obj);
      unsigned i=0;
      while(it.next()){
	ar[i++]=&it;
	unlink(&it,obj);
      }
      qsort(ar,count,sizeof(void *),
	    (int (*)(const void*, const void*))compare);
      for(i=0;i<count;i++)
	append(static_cast<_Tp *>(ar[i]),obj);
    }
};


template<class _Tp>
  static _Tp *rs_alloc(RS *rs){
  RSType type=_Tp::RS_TYPE;
  if(type>31)
    return 0;
  if(!rs->obj_pool[type])
    rs->obj_pool[type]=new SpaceHolder<_Tp>(rs->pool);
  return (static_cast<SpaceHolder<_Tp>*>(rs->obj_pool[type]))->alloc();
}



// Some Base iterator for our needs
template <class _Tp>
  class _RSIteratorBase {
    protected:
      RSObj *root;
      RSLinkGroup *lg;
      RSLink *nxt;
      RSLink *cur;
      RSType type;
    public:
      _RSIteratorBase(RSObj *_root) : 
	root(_root),lg(0),nxt(0),cur(0),type(_Tp::RS_TYPE) {}
      _Tp &operator*() { return *(static_cast<_Tp *>(cur->obj)); }
      _Tp *operator->() { return static_cast<_Tp *>(cur->obj); }
      _Tp *operator&() { return static_cast<_Tp *>(cur->obj); }
  };

// Goes from initial till the last element,
// precalculate next element, so current element
// can be relinked during loops.

template <class _Tp>
  class RSIterator : public _RSIteratorBase<_Tp> {
    private:
      void _find_first(){
	while(this->lg && !this->lg->HasA(this->type))
	  this->lg=this->lg->nxt;
	if(!this->lg)
	  return;
	this->nxt=this->lg->cld;
      }
      void _find_next(){
	if(this->nxt->nxt){
	  this->nxt=this->nxt->nxt;
	  return;
	}
	this->lg=this->lg->nxt;
	_find_first();
      } 
    public:
      RSIterator(RSObj *_root) : _RSIteratorBase<_Tp>(_root) {
	this->lg=this->root->cld;
	_find_first();
      }
      bool next() {
	if(!this->nxt || (this->nxt==this->cur))
	  return false;
	this->cur=this->nxt;
	_find_next();
	return true;
      }
      bool valid() const {
	return (this->nxt && (this->nxt!=this->cur));
      }
  };

template <class _Tp>
  class _RSParentIteratorBase {
    protected:
      RSObj *root;
      RSLink *nxt;
      RSLink *cur;
      RSType type;
    public:
      _RSParentIteratorBase(RSObj *_root) : 
	root(_root),nxt(0),cur(0),type(_Tp::RS_TYPE) {}
      _Tp &operator*() { return *(static_cast<_Tp *>(cur->prt->prt)); }
      _Tp *operator->() { return static_cast<_Tp *>(cur->prt->prt); }
      _Tp *operator&() { return static_cast<_Tp *>(cur->prt->prt); }
  };


template <class _Tp>
  class RSParentIterator : public _RSParentIteratorBase<_Tp> {
    private:
      void _find_first(){
	while(this->nxt && !this->nxt->prt->prt->IsA(this->type))
	  this->nxt=this->nxt->nxt_lnk;
      }
      void _find_next(){
	this->nxt=this->nxt->nxt_lnk;
	_find_first();
      } 
    public:
      RSParentIterator(RSObj *_root) : _RSParentIteratorBase<_Tp>(_root) {
	this->nxt=this->root->prt;
	_find_first();
      }
      bool next() {
	if(!this->nxt || (this->nxt==this->cur))
	  return false;
	this->cur=this->nxt;
	_find_next();
	return true;
      }
  };


template <class _Tp>
  class RSDeepIterator : public _RSIteratorBase<_Tp> {
    private:
      unsigned max_depth;
      unsigned cdepth;
      RSLinkGroup *clg[32];
      RSLink      *clnk[32];
      bool _find_first(unsigned depth){
	if(depth<max_depth){
	  RSLink *lnk=clnk[depth];
	  while(lnk){
	    RSObj *cobj=lnk->obj;
	    clg[depth+1]=cobj->cld;
	    if(cobj->cld){
	      clnk[depth+1]=cobj->cld->cld;
	      if(_find_first(depth+1))
		return true;
	    }
	    if(clg[depth]->HasA(this->type)){
	      this->nxt=lnk;
	      cdepth=depth;
	      return true;
	    }
	    lnk=lnk->nxt;
	  }
	} else if(clg[depth]->HasA(this->type)){
	  this->nxt=clnk[depth];
	  cdepth=depth;
	  return true;
	}
	return false;
      }
      void _find_next(){
	while(1){
	  while(clnk[cdepth]->nxt){
	    clnk[cdepth]=clnk[cdepth]->nxt;
	    if(_find_first(cdepth))
	      return; // success
	  }
	  // try go right
	  clg[cdepth]=clg[cdepth]->nxt;
	  if(!clg[cdepth])
	    break;
	  clnk[cdepth]=clg[cdepth]->cld;
	  if(_find_first(cdepth))
	    return;
	}
	// have to go up if possible
	if(cdepth){
	  cdepth--;
	  if(clg[cdepth]->HasA(this->type)){ // can't use _find_first here...
	    this->nxt=clnk[cdepth];
	    return; // success
	  }
	  _find_next();
	}
	// failure
      } 
    public:
      RSDeepIterator(RSObj *_root,int _max_depth=31) : 
	_RSIteratorBase<_Tp>(_root),
	max_depth(_max_depth),cdepth(0) {
	if(max_depth>31)
	  max_depth=31;
	clg[cdepth]=this->root->cld;
	if(this->root->cld){
	  clnk[cdepth]=this->root->cld->cld;
	  if(_find_first(cdepth))
	    return;
	  _find_next();
	}
      }
      bool next() {
	if(!this->nxt || (this->nxt==this->cur))
	  return false;
	this->cur=this->nxt;
	_find_next();
	return true;
      }
  };

#endif

#ifdef TR_LOCAL

// Test

class Hit : public RSObj {
  public:
    static const unsigned RS_TYPE = 0;
  protected:
    Hit() : RSObj(RS_TYPE) {}
};

class TPCHit : public Hit {
  public:
    static const unsigned RS_TYPE = 1;

    TPCHit() : Hit() { AddRSType(RS_TYPE); }
};

class CalHit : public Hit {
  public:
    static const unsigned RS_TYPE = 2;

  protected:
    CalHit() : Hit() { AddRSType(RS_TYPE); }
};

class ECalHit : public CalHit {
  public:
    static const unsigned RS_TYPE = 3;

    ECalHit() : CalHit() { AddRSType(RS_TYPE); }
};

class HCalHit : public CalHit {
  public:
    static const unsigned RS_TYPE = 4;

    HCalHit() : CalHit() { AddRSType(RS_TYPE); }
};

class Cluster : public RSObj {
  public:
    static const unsigned RS_TYPE = 5;

    Cluster() : RSObj(RS_TYPE) {}
};

class Track : public Cluster {
  public:
    static const unsigned RS_TYPE = 6;
  
    Track() : Cluster() { AddRSType(RS_TYPE); }
};


int main(int argc,const char *argv[]){
  HCalHit h,h1,h2;
  ECalHit eh,eh1;
  Track t;
  RSObj *o=&h;
  Cluster cl;
  RS rs;
  rs.append(&h,&t);
  rs.append(&h1,&t);
  rs.append(&h2,&t);
  rs.append(&cl,&t);
  rs.append(&eh,&cl);
  rs.append(&eh1,&cl);

  //  rs.alloc<HCalHit>();
  ECalHit *eh5=rs.alloc<ECalHit>();
  rs.append(eh5,&cl);
  //rs.append(sh_array<ECalHit>(rs.pool,1),&t);

  cout<<"Is HCalHit Hit? "<<o->IsA<Hit>()<<endl;
  o=&t;
  cout<<"Is Track hit? "  <<o->IsA<Hit>()<<endl;
  cout<<"Is Track Cluster? "<<o->IsA<Cluster>()<<endl;
  cout<<"Number of hits in the track: "<<o->getNumberOf<Hit>()<<endl;
  cout<<"Number of clusters in the track: "<<o->getNumberOf<Cluster>()<<endl;

  RSDeepIterator<Hit> hi(&t);
  while(hi.next()){
    Hit &hhh=*hi;
    cout<<"Hit found!"<<endl;
  }
  cout<<endl;
  RSIterator<Hit> si(&t);
  while(si.next())
    cout<<"Hit found!"<<endl;
  RSDeepIterator<ECalHit> ehi(&t);
  while(ehi.next()){
    ECalHit &ech=*ehi;
    cout<<" Ecal Hit found!"<<endl;
  }
  RSDeepIterator<HCalHit> hhi(&t);
  while(hhi.next())
    cout<<"   HCal Hit found!"<<endl;

  return 0;
}

#endif
