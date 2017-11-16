// reference_counter:
//   

#ifndef _REFERENCE_COUNTER_H_
#define _REFERENCE_COUNTER_H_

#include "accounting_ptr.h"
#include <cassert>

class reference_counter
{
    template <class a_reference_counter> friend class accounting_ptr;

private:
    reference_counter *operator&() {return this;};

    reference_counter *self;

    int counter;

public:
     reference_counter()
       : self(this),
      counter(0) 
	{
	};

     virtual ~reference_counter()
	 {
	     counter=0;
	 };

     int references() const {return counter;};

     int isUsed() const {return counter!=0;};

protected:

     int unref() {	     
       assert (counter > 0);
       return --counter;
     };


     reference_counter *ref() {
	 counter++; 
	 return this;
     };
};

#endif
