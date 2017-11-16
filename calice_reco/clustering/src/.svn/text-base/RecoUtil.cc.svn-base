#include "RecoUtil.hh"

template <class _Tp>
_Tp *sh_array(SpacePool &pool,unsigned n) {
  return (new SpaceHolder<_Tp>(pool))->array(n);
}

template <class _Tp>
_Tp *fsh_array(SpacePool &pool,unsigned n) {
  return (new FastSpaceHolder<_Tp>(pool))->array(n);
}

RSLink *RSObj::getPrtLink(RSObj *_prt){
  RSLink *lnk=prt;
  while(lnk){
    if(lnk->prt->prt==_prt)
      return lnk;
    lnk=lnk->nxt_lnk;
  }
  return 0;
}

void RSObj::remove(RSLinkGroup *lg){
  if(lg->prt!=this)
    return;
  if(cld==lg)
    cld=lg->nxt;
  else {
    RSLinkGroup *clg=cld;
    while(clg->nxt!=lg)
      clg=cld->nxt;
    clg->nxt=lg->nxt;
  }
  lg->prt=0;
  lg->nxt=0;
}


unsigned RSObj::getNumberOf(RSType type){
  RSLinkGroup *lg=cld;
  unsigned size=0;
  while(lg){
    if(lg->HasA(type))
      size+=lg->size;
    lg=lg->nxt;
  }
  return size;
}

// Not so nice, but can't write templated functions otherwise...
RSObj* RSObj::getParent(RSType type) {
  RSLink *lnk=prt;
  while(lnk){
    RSLinkGroup *lg=lnk->prt;
    RSObj *obj=lg->prt;
    if(obj->IsA(type))
      return obj;
    lnk=lnk->nxt_lnk;
  }
  return 0;
}

  bool RSLinkGroup::HasA(RSType type) { 
    return (rs_type_mask & (1<<type))?true:false; 
  }

  void RSLinkGroup::init(RSObj *_prt,RSTypeMask tmask) {
    prt=_prt;
    nxt=prt->cld;
    prt->cld=this;
    cld=lcld=0;
    rs_type_mask=tmask;
    size=0;
  }
  
  void RSLinkGroup::prepend(RSLink *lnk){
    lnk->prt=this;
    lnk->nxt=cld;
    lnk->prv=0;
    if(cld)
      cld->prv=lnk;
    else
      lcld=lnk;
    cld=lnk;
    size++;
  }
  
  void RSLinkGroup::append(RSLink *lnk){
    lnk->prt=this;
    lnk->nxt=0;
    lnk->prv=lcld;
    if(lcld)
      lcld->nxt=lnk;
    else
      cld=lnk;
    lcld=lnk;
    size++;
  }
  
  void RSLinkGroup::remove(RSLink *lnk){
    if(lnk->prt!=this)
      return;
    if(lnk->prv)
      lnk->prv->nxt=lnk->nxt;
    else
      cld=lnk->nxt;
    if(lnk->nxt)
      lnk->nxt->prv=lnk->prv;
    else
      lcld=lnk->prv;
    size--;
    lnk->prv=lnk->nxt=0;
    lnk->prt=0;
    if(!lnk->obj)
      return;
    if(lnk->obj->prt == lnk)
      lnk->obj->prt=lnk->nxt_lnk;
    else {
      RSLink *olnk=lnk->obj->prt;
      while(olnk->nxt_lnk!=lnk)
	olnk=olnk->nxt_lnk;
      olnk->nxt_lnk=lnk->nxt_lnk;
    }
    lnk->nxt_lnk=0;
    lnk->obj=0;
  }


