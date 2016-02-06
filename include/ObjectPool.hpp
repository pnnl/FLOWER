///////////////////////////////////////////////////////////////////////////////
// COPYRIGHT (C) 2008.  PNNL.  All Rights Reserved.
//
// THIS FILE INITIALLY CREATED WITH:
//     TEMPLATE NAME:  lang_cpp_hdr.template
//     COMMAND NAME:   gensrc
//
// CODING CONVENTIONS:
//    * Class names are CamelCase with first word upper case
//    * Functions are camelCase with first word lower case
//    * Function parameters are lower case with _ and have p_ prefix
//    * Member variables always use 'this' pointer
///////////////////////////////////////////////////////////////////////////////


#ifndef OBJECTPOOL_HPP
#define OBJECTPOOL_HPP

// System Includes
#include <vector>
// External Includes
#include <Diagnostic.hpp>
// Internal Includes
// Application Includes


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "ObjectPool"

// Uncomment this to stub out all MT locking
//#define BOOST_NO_MT


// Namespaces used
using namespace std;


template <class T>
class ObjectPool
{

private:

  // Getters/Setters

  inline int unsigned getMaxSize(void) const throw()
  {
    return(this->max_size);
  }


  // Constructors

  ObjectPool(void);
  ObjectPool(ObjectPool const & p_object_pool);


  // Variables 

  vector< std::shared_ptr<T> > objs;
  int unsigned max_size;


public:

  // Constructors

  ObjectPool(int unsigned const p_max_size, T const & p_obj) throw() :
    max_size(p_max_size + 1)
  {
    objs.reserve(getMaxSize());
    for (int unsigned idx = 0; idx < getMaxSize(); ++idx)
    {
      objs.push_back(std::shared_ptr<T>(new T(p_obj)));
    }
    return;
  }


  // Destructors

  ~ObjectPool(void) throw()
  {
    return;
  }


  // Functions

  bool release(std::shared_ptr<T> const & p_object) throw()
  {
    if (objs.size() < getMaxSize())
    {      
      objs.push_back(p_object);
      return(true);
    }
    else
    {
      ERROR(RangeError, "Releasing object back to ObjectPool", "Too many objects already in ObjectPool");
      return(false);
    }

  }


  bool acquire(std::shared_ptr<T> & p_ptr) throw()
  {
    if (! objs.empty())
    {
      //p_ptr = objs.at(objs.size() - 1);
      p_ptr = objs.back();
      objs.pop_back();
      return(true);
    }

    ERROR(RangeError, "Getting object from ObjectPool", "No more objects available");
    return(false);
  }


  int unsigned size(void) const throw()
  {
    return(objs.size());
  }


  std::shared_ptr<T> at(int unsigned const p_idx) throw()
  {
    return(objs.at(p_idx));
  }


  std::shared_ptr<T> const at(int unsigned const p_idx) const throw()
  {
    return(objs.at(p_idx));
  }

};

#endif

/*

//  DEVELOPER NOTE: Just in case we want to try a true Stack implementation rather than
//                  mimicking a Stack with Vector
#ifndef _Stack_
#define _Stack_
#define MaxStackSize          100     //Change the size depending on how big you need it

template <class ClassElem>
class Stack
{
public:
     Stack();                              //Constructor
     bool Get(const int &pos, ClassElem& element);          //Choose which element in the stack you want
     bool Insert(const ClassElem &elem);     //Insert a new element
     ClassElem Pop();                              //Pop the stack
     ClassElem Top();                              //Returns last element in the stack
     bool isEmpty();                              //Returns true if empty false if not

private:

     ClassElem stackArray [MaxStackSize];     //Array for storing elements

     int topIndex;                                   //Keeps track of the position
};

template < class ClassElem >
Stack<ClassElem>::Stack()                         //Constructor
{
     topIndex = -1;                    //Top of the stack is at -1
}

template < class ClassElem >
bool Stack<ClassElem>::Insert(const ClassElem &elem) //Inserts new element
{
     topIndex++;                    //Top of stack +1 (remember that constructor initializes
                                        //it to -1 so the first time it runs its 0
     stackArray[topIndex] = elem; //Inserts the new element
}

template < class ClassElem >
ClassElem Stack<ClassElem>::Pop()     //Pops the stack
{
     int returnIndex(topIndex);     //Return index == topindex
     --topIndex;          //topIndex -1
     return stackArray[returnIndex];
}

template < class ClassElem>
ClassElem Stack<ClassElem>::Top()     //returns the lement at the top of the stack
{
     return stackArray[topIndex];
}

template < class ClassElem >
bool Stack<ClassElem>::Get(const int &pos, ClassElem& element)          //Chose which element you want to get
{
     if (pos > topIndex)
          return -1;               //returns -1 if 
     else
          element = stackArray[pos];     //else assigns element the value of the stack element
}

#endif
*/
