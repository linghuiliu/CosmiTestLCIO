#include <TLineIterator.hh>
#include <iostream>

string TLineIterator::GetWord(bool allow_start_with_non_alpha) throw(runtime_error)
{
  if (!UnSpace()) throw runtime_error("Missing character");
  
  string a_word;
  const Char_t *ptr=fPtr;
  if (!isalpha(*ptr)) {
    if (!allow_start_with_non_alpha || (!isdigit(*ptr) && !ispunct(*ptr) && *ptr!='_' && *ptr!='-' && *ptr!='+')) {
      std::stringstream message;
      message << "TLineIterator::getWord> Word starts with not allowed character '" 
	      << ( *ptr>32 ? *ptr : '?' ) 
	      << "'. Allowed first charcaters are : [a-zA-Z].";
      throw runtime_error(message.str());
    }
  }
  if (!allow_start_with_non_alpha)  {
    a_word+=*ptr;
    ptr++;
  }
  while (isalpha(*ptr) || isdigit(*ptr) || ispunct(*ptr) || *ptr=='_' || *ptr=='-' || *ptr=='+') {
    a_word+=*ptr;
    ptr++;
  }
  if (*ptr && !isspace(*ptr)) {
    std::stringstream message;
    message << "TLineIterator::getWord> Word contains not allowed character " 
	    << ( *ptr>32 ? *ptr : '?' ) 
	    << ". Allowed charcaters: [a-zA-Z][a-zA-Z0-9+-_]*([[:space:]]|\n)";
    throw runtime_error(message.str());
  }
  fPtr=ptr;
  return a_word;
}

void TLineIterator::GetWord(string &dest, bool allow_start_with_non_alpha) throw(runtime_error)
{
  if (!UnSpace()) throw runtime_error("Missing character");

  const Char_t *ptr=fPtr;
  if (!isalpha(*ptr)) {
    if (!allow_start_with_non_alpha || (!isdigit(*ptr) && !ispunct(*ptr) && *ptr!='_' && *ptr!='-' && *ptr!='+')) {
      std::stringstream message;
      message << "TLineIterator::getWord> Word starts with not allowed character '" 
	      << ( *ptr>32 ? *ptr : '?' ) 
	      << "'. Allowed first charcaters are : [a-zA-Z].";
      throw runtime_error(message.str());
    }
  }
  if (!allow_start_with_non_alpha)  {
    dest+=*ptr;
    ptr++;
  }
  while (isalpha(*ptr) || isdigit(*ptr) || ispunct(*ptr) || *ptr=='_' || *ptr=='-' || *ptr=='+') {
    dest+=*ptr;
    ptr++;
  }
  
  if (*ptr && !isspace(*ptr)) {
    std::stringstream message;
    message << "TLineIterator::getWord> Word contains not allowed character " 
	    << ( *ptr>32 ? *ptr : '?' ) 
	    << ". Allowed charcaters: [a-zA-Z][a-zA-Z0-9+-_]*([[:space:]]|\n)";
    throw runtime_error(message.str());
  }
  fPtr=ptr;
}

UInt_t TLineIterator::GetBinary() 
{
    
  const Char_t *ptr=fPtr;
  if (!UnSpace()) return false;
  UInt_t value=0;
  while ((*ptr)=='0' || (*ptr=='1') || (*ptr)=='_') {
    value<<=1;
    if (*ptr=='1') {
      value+=1;
    }
    ptr++;
  }

  if (!(*ptr) || isspace(*ptr)) {
    fPtr=ptr;
    UnSpace();
    return value;
  }
  else {
    throw runtime_error("Not a binary number.");
  }
}

UInt_t TLineIterator::GetHex() 
{
  const Char_t *ptr=fPtr;
  if (!UnSpace()) return false;
  if ((*ptr=='0')) ptr++;
  if ((*ptr=='x')) ptr++;
  while (isdigit(*ptr) || (*ptr>='A' && *ptr<='F') || (*ptr>='a' && *ptr<='f')) ptr++;
  
  if (!(*ptr) || isspace(*ptr)) {

    unsigned int hex_val;
    sscanf(fPtr,"%x",&hex_val);

    fPtr=ptr;
    return hex_val;
  }
  else {
    throw runtime_error("Not a hexadecimal number");
  }
}
