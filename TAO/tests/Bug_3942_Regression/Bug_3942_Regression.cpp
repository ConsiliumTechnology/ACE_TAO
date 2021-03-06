#include "FooS.h"
#include "tao/LocalObject.h"
#include "tao/Version.h"

#if TAO_MAJOR_VERSION < 2 && TAO_MINOR_VERSION < 6
# define RV refcount_.value
#else
# define RV _refcount_value
#endif

#define ASSERT_REFCOUNT(count, message)                                       \
  if (ximpl->RV () != count)                                                  \
    {                                                                         \
      ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%l %C: expected %d actual %d\n"), \
                        message, count, ximpl->RV ()), -1);                   \
    }

struct XImpl
#ifdef LOCAL
  : X
# if TAO_MAJOR_VERSION < 2 && TAO_MINOR_VERSION < 6
// FUZZ: disable check_for_TAO_Local_RefCounted_Object
  , TAO_Local_RefCounted_Object
# endif
#else
  : POA_X
#endif
{
#if defined LOCAL && (TAO_MAJOR_VERSION < 2 && TAO_MINOR_VERSION < 6)
  unsigned long _refcount_value () const { return refcount_.value (); }
#endif

  void op () {}
};

int ACE_TMAIN (int, ACE_TCHAR *[])
{
#ifdef LOCAL
  XImpl* ximpl = new XImpl;
  X_var x = ximpl;
#else
  int zero_args = 0;
  CORBA::ORB_var orb = CORBA::ORB_init (zero_args,
                                        static_cast<ACE_TCHAR **> (0));
  CORBA::Object_var obj = orb->resolve_initial_references ("RootPOA");
  PortableServer::POA_var poa = PortableServer::POA::_narrow (obj);
  XImpl* x_servant = new XImpl;
  PortableServer::ServantBase_var sbv (x_servant);
  X_var x = x_servant->_this ();
  X* ximpl = x;
#endif

  ASSERT_REFCOUNT (1, "baseline");

  {
    Y y;
    y.theX = x;
    ASSERT_REFCOUNT (2, "in struct");
    X_var x2 = y.theX;
    ASSERT_REFCOUNT (3, "from struct");
  }

  ASSERT_REFCOUNT (1, "baseline reestablished");

  {
    XSeq xseq (1);
    xseq.length (1);
    xseq[0] = x;
    ASSERT_REFCOUNT (2, "in sequence");
#ifdef _MSC_VER
    // For now, only attempt to compile this on MSVC -- it doesn't compile
    // on GCC (and possibly others) due to:
    // error: conversion from 'TAO::details::object_reference_sequence_element<TAO::details::object_reference_traits<X, TAO_Objref_Var_T<X>, true> >' to non-scalar type 'X_var' requested
    // When the bug is fixed, the #ifdef should be removed.  Until then, the
    // test will appear to pass on other compilers, but the MSVC failures should
    // be enough to notice that the bug needs fixing.
    X_var x2 = xseq[0];
    if (ximpl->RV () < 3) x2._retn (); // avoid a crash
    ASSERT_REFCOUNT (3, "from sequence");
#endif
  }

  return 0;
}
