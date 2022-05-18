///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute.
//
// CODING CONVENTIONS:
//    * Class names are CamelCase with first word upper case
//    * Functions are camelCase with first word lower case
//    * Function parameters are lower case with _ and have p_ prefix
//    * Member variables always use 'this' pointer
///////////////////////////////////////////////////////////////////////////////


#ifndef EVENT_HPP
#define EVENT_HPP


// System Includes
#include <exception>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
// External Includes
#include <boost/lexical_cast.hpp>
// Internal Includes
// Application Includes


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Event"


// Namespaces
using namespace std;
using boost::lexical_cast;


//
// DEVELOPER NOTE:
//   Based on URL: http://www.codeproject.com/KB/cpp/accppdelegates.aspx
//
// DEVELOPER NOTE:
//   From Jason Christy on 03/25/08:
//   I tried to separate the method definitions from there declarations by
//   placing the latter within its own .cpp file.  Unfortunately, I was unable
//   to do so. The only reason for this separation is to conform with current
//   coding styles that favors separation of declaration and definition.
//
// DEVELOPER NOTE:
//   From Darren Curtis on 12/18/08:
//   The "Return" will most likely always be of type void. The problem is
//   that you can't create a member variable of type Return when Return
//   is void. One scenario is to use this class as a "connector" with
//   a single object using a non-void Return type. The second scenario
//   is to use this class as a "fan out" with multiple objects using
//   void Return type.
//
// Event class - invoke operator() on a collection of objects.
// Note that only one argument is supported for the parameters of
// the target signature. However, it is trivial to extend this to
// multiple arguments by having multiple templates similar to this
// one.


template <typename Return, typename Data>
class Event
{

private:

  class Base // To specify the signature
  {

  public:

    // Constructors

    Base(string const & p_name) noexcept(true) :
      name(p_name)
    {
      return;
    }


    // Destructor

    virtual ~Base(void) noexcept(true)
    {
      // Required for polymorphic destruction to work properly
      return;
    }


    // Operators

    // Target signature. Invoked through base class pointers
    virtual Return operator()(Data const & p_data) = 0;


    virtual string getName(void) const noexcept(true)
    {
      return(this->name);
    }


  private:

    // Constructors

    Base(Base const & p_event);


    // Operators

    Base & operator=(Base const & p_event);


    // Variables

    string name;

  };  // End Base class
    

  inline int unsigned nextIndex(void) noexcept(true)  // Cannot be const
  {
    int unsigned temp = this->index;
    ++this->index;
    return(temp);
  }


  // Constructors

  // Can't allow copying unless we modify the class to make a deep
  // copy of each of the pointers it holds. Base class would need 
  // a virtual Clone() method to allow polymorphic copying.
  Event(Event const & p_event);


  // Operators

  Event & operator=(Event const & p_event);


  // Variables

  map<int unsigned, Base *> delegates;
  map<string, int unsigned> lookup;
  map<int unsigned, string> reverse_lookup;
  int unsigned              index;
  string                    eventName;


public:  

  // Derived class - parametrise the implementation so we can specify the 
  // target class in client code 
  template <typename Class>
  class New : public Base
  {

  public:

    // Signature applied to a pointer-to-member for target class
    typedef Return (Class::*Func)(Data);


    // Constructor

    New(string const & p_name, Class * p_instance, Func const & p_function) noexcept(true) :
      Base(p_name),
      delegate_instance(p_instance),
      member_function(p_function)
    {
      return;
    }
     

    // Destructor

    ~New(void) noexcept(true)
    {
      this->delegate_instance  = NULL;
      this->member_function    = NULL;
      return;
    }


    // Operators

    virtual Return operator()(Data const & p_data) noexcept(true)
    {
      // Invoke the function through the pointer-to-member
      return(delegate_instance->*member_function)(p_data);
    }


  private:

    // Variables

    Class * delegate_instance; // Pointer to the object we are delegating to
    Func    member_function;   // Address of the function on the delegate object

  };  // End New class
    

  // Derived class - this is not parameterised and is intended for 
  // static and free functions
  class Static : public Base
  {

  public:

    typedef Return (*Func)(Data);


    // Constuctors

    Static(string const & p_name, Func const & p_function) noexcept(true) :
      Base(p_name),
      member_function(p_function)
    {
      return;
    }
      

    // Destructor

    ~Static(void) noexcept(true)
    {
      this->member_function = NULL; 
      return;
    }


    // Operators

    virtual Return operator()(Data const & p_data) noexcept(true)
    {
      // Invoke the function throught the pointer-to-member
      return(member_function(p_data));
    }


  private:

    // Variables

    Func member_function; 

  };  // End Static class
    

  // Constructors

  Event(string const & p_name = "No Event Name Available") noexcept(true) :
    index(0),
    eventName(p_name)
  {
    return;
  }


  // Destructor

  ~Event(void) noexcept(true)
  {
    typename map<int unsigned, Base *>::iterator end = this->delegates.end();
    typename map<int unsigned, Base *>::iterator itor;

    for (itor = this->delegates.begin(); itor != end; ++itor)
    {
      delete((*itor).second); 
    }

    return;
  }


  // Operators 

  Event & operator+=(Base * p_function) noexcept(true)
  {
    // Add a new target (callee) to our list
    int unsigned index = nextIndex();
    this->lookup.insert(make_pair(p_function->getName(), index));
    this->delegates.insert(make_pair(index, p_function));
    this->reverse_lookup.insert(make_pair(index, p_function->getName()));
    return(*this);
  }


  Event & operator-=(string const & p_name) noexcept(true)
  {
    map<string, int unsigned>::iterator l_end  = this->lookup.end();
    map<string, int unsigned>::iterator l_itor = this->lookup.find(p_name);
    bool found = false;

    if (l_end != l_itor)
    {
      typename map<int unsigned, Base *>::iterator d_end  = this->delegates.end();
      typename map<int unsigned, Base *>::iterator d_itor = this->delegates.find((*l_itor).second);

      if (d_end != d_itor)
      {
        this->lookup.erase(l_itor);
        this->reverse_lookup.erase((*d_itor).first);
        this->delegates.erase(d_itor);
        found = true;
      }
    }

    if (! found)
    {
      printError(NotFound, "Can't find delegate: ", p_name);
    }

    return(*this);
  }


  Return  operator()(Data const & p_data) noexcept(true)
  {
    // Call all the targets - there will be horrible undefined behaviour
    // if the callee object no longer exists. You have been warned!
    // DEVELOPER NOTE: You can only return the output from the LAST delegate
    typename map<int unsigned, Base *>::iterator end       = this->delegates.end();
    typename map<int unsigned, Base *>::iterator itor      = this->delegates.begin();
    typename map<int unsigned, Base *>::iterator itor_test = this->delegates.begin();

    while (itor != end)
    {
      try
      {
        ++itor_test;
        if (end == itor_test)
        {
          return((*(*itor).second)(p_data));
        }
        (*(*itor).second)(p_data); 
      }
      catch (std::exception & e)
      {
        printError(UnknownException, getMessage((*itor).first), e.what());
      }
      catch (...)
      {
        printError(UnknownException, getMessage((*itor).first));
      }
      ++itor;
    }

    // DEVELOPER NOTE: Should never get here but if we do...
    return((Return)NULL);
  }


  // Public Functions

  void printError(string const & errorType, string const & p_message, string const & p_extra = "N/A") const noexcept(true)
  {
#ifdef ERROR_MSG
    ERROR_MSG(UnknownException, getContext().c_str(),  (p_message +  p_extra).c_str());
#else
    cerr << "ERROR_MSG: " <<    getContext() << ", " << p_message << p_extra << endl;
#endif
    return;
  }


  Return call(Data const & p_data) noexcept(true)
  {
    // Just some syntactic sugar so that the user can use
    //   delegate->call(data)
    // instead of
    //   delegate->operator()(data)
    return(this->operator()(p_data));
  }


  inline string getEventName(void) const noexcept(true)
  {
    return(this->eventName);
  }


  inline string getContext(void) const noexcept(true)
  {
    return("Event (" + getEventName() + ")");
  }


  inline string getMessage(int unsigned const p_idx) const noexcept(true)
  {
    map<int unsigned, string>::const_iterator itor = this->reverse_lookup.find(p_idx);

    if (itor != this->reverse_lookup.end())
    {
      return("Exception from delegate(" + (*itor).second + "): ");
    }

    return("");
  }


  vector<string> getDelegateNames(void) noexcept(true)
  {
    map<int unsigned, string>::iterator end  = this->reverse_lookup.end();
    map<int unsigned, string>::iterator itor = this->reverse_lookup.begin();
    map<string, int unsigned>::iterator l_itor;
    vector<string> names;

    for (itor = this->reverse_lookup.begin(); itor != end; ++itor)
    {
      l_itor = this->lookup.find((*itor).second); 
      names.push_back(((*l_itor).first + "(" + lexical_cast<string>((*itor).first) + ")")); 
    }

    return(names);
  }


  string printDelegateNames(string const & p_separator = ",", string const & p_prefix = "") noexcept(true)
  {
    string result = getEventName() + ":";
    vector<string> names = getDelegateNames();
    vector<string>::iterator end = names.end();
    vector<string>::iterator itor;

    for (itor = names.begin(); itor != end; ++itor)
    {
      result += p_prefix + (*itor) + p_separator;
    }

    return(result);
  }


  inline int unsigned size(void) const noexcept(true)
  {
    return(this->delegates.size());
  }

};

#endif // EVENT_HPP
