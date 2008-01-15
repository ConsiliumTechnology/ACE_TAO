// $Id$

// ============================================================================
//
// = LIBRARY
//   TAO/tests/Timeout
//
// = FILENAME
//   test_i.h
//
// = AUTHOR
//   Carlos O'Ryan
//
// ============================================================================

#ifndef TAO_TIMEOUT_TEST_I_H
#define TAO_TIMEOUT_TEST_I_H

#include "testS.h"

class Simple_Server_i : public POA_Simple_Server
{
  // = TITLE
  //   Simpler Server implementation
  //
  // = DESCRIPTION
  //   Implements the Simple_Server interface in test.idl
  //
public:
  Simple_Server_i (CORBA::ORB_ptr orb);
  // ctor

  // = The Simple_Server methods.
  void echo (CORBA::Long x,
                    CORBA::Long msecs, const char* msg)
    ACE_THROW_SPEC ((CORBA::SystemException));
  void shutdown (void)
    ACE_THROW_SPEC ((CORBA::SystemException));

private:
  CORBA::ORB_var orb_;
  // The ORB

  int echo_count_;
};

#endif /* TAO_TIMEOUT_TEST_I_H */
