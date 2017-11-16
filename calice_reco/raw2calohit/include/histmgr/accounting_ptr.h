// accounting_ptr:
//   

#ifndef _ACCOUNTING_PTR_H_
#define _ACCOUNTING_PTR_H_

class reference_counter;

template <class a_reference_counter>
class accounting_ptr
{

 template <class a_reference,class b_reference> friend class accounting_ptr_x;
 template <class b_rc> friend class accounting_ptr;

 private:
    a_reference_counter *ptr;
    void free() {if (ptr && !ptr->unref()) delete ptr;};
    a_reference_counter *copy() const {return ((ptr) ? (a_reference_counter *) ptr->ref(): 0);};

 public:
    accounting_ptr(accounting_ptr const &a_ptr) 
	{

	  ptr=a_ptr.copy();
	}

    template <class b_rc> accounting_ptr(accounting_ptr<b_rc> const &a_ptr) 
	{
	    ptr=a_ptr.copy();
	}

    accounting_ptr() 
	{
	    ptr=0;
	}

    accounting_ptr(const a_reference_counter *a_class){
	if (a_class)
	    ptr=(a_reference_counter *) ((a_reference_counter *) a_class)->ref();
	else
	    ptr=0;
    }

    ~accounting_ptr()  {free();}
    
    int is_valid() const {return (ptr!=0);}
    bool operator!() const {return (ptr==0);}

    bool operator==(accounting_ptr const &a_ptr) const {return ptr==a_ptr.ptr; }
    bool operator!=(accounting_ptr const &a_ptr) const {return ptr!=a_ptr.ptr; }

    const a_reference_counter *operator->() const {return ptr;}
    a_reference_counter *operator->() {return ptr;}

    template <class a_reference> 
    accounting_ptr operator=(const a_reference_counter *a_class)
	{   

	  if (ptr != static_cast<a_reference_counter *>(a_class)) free();
	  ptr=static_cast<a_reference_counter *>(a_class->ref()); return *this;
	}

    template <class a_reference> 
    accounting_ptr operator=(const accounting_ptr<a_reference> &a_ptr)
	{   
	    if (ptr!=a_ptr.ptr) {free(); ptr=a_ptr.copy(); } return *this;
	}

    accounting_ptr operator=(const accounting_ptr &a_ptr)
	{   
	    if (ptr!=a_ptr.ptr) {free(); ptr=a_ptr.copy(); } return *this;
	}

    a_reference_counter &operator*() {return *ptr;}

    a_reference_counter const &operator*() const {return *ptr;}
};

#endif
