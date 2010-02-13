//
// testdotat.hpp - .@ Langauge
// by £ukasz Szpakowski
//

#ifndef _TESTDOTAT_HPP
#define _TESTDOTAT_HPP

#include <cppunit/Test.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>
#include <sstream>
#include "dotat.hpp"

//
// TestRefPtr
//

class TestRefPtr
  : public CppUnit::TestFixture
{
public:
  class MyObj
    : public dotat::GCObj
  {
    bool &m_b;
  public:
    MyObj(bool &b)
      : m_b(b)
    {
      m_b=false;
    }

    virtual ~MyObj();
  };

  void setUp();
  void tearDown();
  void test_one_reference();
  void test_two_references();

  CPPUNIT_TEST_SUITE(TestRefPtr);
  CPPUNIT_TEST(test_one_reference);
  CPPUNIT_TEST(test_two_references);
  CPPUNIT_TEST_SUITE_END();
};

//
// TestScope
//

class TestScope
  : public CppUnit::TestFixture
{
public:
  void setUp();
  void tearDown();
  void test_define_and_undefine_variables();

  CPPUNIT_TEST_SUITE(TestScope);
  CPPUNIT_TEST(test_define_and_undefine_variables);
  CPPUNIT_TEST_SUITE_END();
};

//
// TestMethod
//

class TestMethod
  : public CppUnit::TestFixture
{
  dotat::Ptr<dotat::Interp> m_interp;
public:
  void setUp();
  void tearDown();
  void test_call_native_method();
  void test_call_native_method_with_data();
  void test_call_method_with_self_expr();
  void test_call_method_with_var_expr();
  void test_call_curring_method();

  CPPUNIT_TEST_SUITE(TestMethod);
  CPPUNIT_TEST(test_call_native_method);
  CPPUNIT_TEST(test_call_native_method_with_data);
  CPPUNIT_TEST(test_call_method_with_self_expr);
  CPPUNIT_TEST(test_call_method_with_var_expr);
  CPPUNIT_TEST(test_call_curring_method);
  CPPUNIT_TEST_SUITE_END();
};

//
// TestObj
//

class TestObj
  : public CppUnit::TestFixture
{
public:
  void setUp();
  void tearDown();
  void test_define_and_undefine_methods();

  CPPUNIT_TEST_SUITE(TestObj);
  CPPUNIT_TEST(test_define_and_undefine_methods);
  CPPUNIT_TEST_SUITE_END();
};

//
// TestExpr
//

class TestExpr
  : public CppUnit::TestFixture
{
  dotat::Ptr<dotat::Interp> m_interp;
  dotat::Ptr<std::ostringstream> m_oss;
public:
  void setUp();
  void tearDown();
  void test_evaluate_val_expr();
  void test_evaluate_var_expr();
  void test_evaluate_self_expr();
  void test_evaluate_send_method_expr();
  void test_define_and_send_method_1_arg();
  void test_define_and_send_method_2_args();
  void test_divided_curring();
  void test_double_evaluation();
  void test_double_evaluation_var_rcvr();
  void test_double_evaluation_var_arg();
  void test_define_and_send_self_method_1_arg();
  void test_define_and_send_self_method_2_args();
  void test_evaluate_determined_method();
  void test_evaluate_undetermined_method();
  void test_detect_infinite_tail_recursion();
  void test_detect_tail_recursion();

  CPPUNIT_TEST_SUITE(TestExpr);
  CPPUNIT_TEST(test_evaluate_val_expr);
  CPPUNIT_TEST(test_evaluate_var_expr);
  CPPUNIT_TEST(test_evaluate_self_expr);
  CPPUNIT_TEST(test_evaluate_send_method_expr);
  CPPUNIT_TEST(test_define_and_send_method_1_arg);
  CPPUNIT_TEST(test_define_and_send_method_2_args);
  CPPUNIT_TEST(test_divided_curring);
  CPPUNIT_TEST(test_double_evaluation);
  CPPUNIT_TEST(test_double_evaluation_var_rcvr);
  CPPUNIT_TEST(test_double_evaluation_var_arg);
  CPPUNIT_TEST(test_define_and_send_self_method_1_arg);
  CPPUNIT_TEST(test_define_and_send_self_method_2_args);
  CPPUNIT_TEST(test_evaluate_determined_method);
  CPPUNIT_TEST(test_evaluate_undetermined_method);
  CPPUNIT_TEST(test_detect_infinite_tail_recursion);
  CPPUNIT_TEST(test_detect_tail_recursion);
  CPPUNIT_TEST_SUITE_END();
};

//
// TestParser
//

class TestParser
  : public CppUnit::TestFixture
{
  dotat::Ptr<dotat::Interp> m_interp;
public:
  void setUp();
  void tearDown();
  void test_parse_val_expr();
  void test_parse_var_expr();
  void test_parse_self_expr();
  void test_parse_send_method_expr();
  void test_parse_expr_chain();
  void test_parse_expr_with_subexprs();
  void test_skip_comment();

  CPPUNIT_TEST_SUITE(TestParser);
  CPPUNIT_TEST(test_parse_val_expr);
  CPPUNIT_TEST(test_parse_var_expr);
  CPPUNIT_TEST(test_parse_self_expr);
  CPPUNIT_TEST(test_parse_send_method_expr);
  CPPUNIT_TEST(test_parse_expr_chain);
  CPPUNIT_TEST(test_parse_expr_with_subexprs);
  CPPUNIT_TEST(test_skip_comment);
  CPPUNIT_TEST_SUITE_END();
};

//
// TestInterp
//

class TestInterp
  : public CppUnit::TestFixture
{
  dotat::Ptr<dotat::Interp> m_interp;
public:
  void setUp();
  void tearDown();
  void test_methods_of_nil_obj();
  void test_methods_of_num_obj();
  void test_methods_of_method_obj();
  void test_methods_of_arg_obj();
  void test_push_and_pop_one_scope();
  void test_push_and_pop_two_scopes();

  CPPUNIT_TEST_SUITE(TestInterp);
  CPPUNIT_TEST(test_methods_of_nil_obj);
  CPPUNIT_TEST(test_methods_of_num_obj);
  CPPUNIT_TEST(test_methods_of_method_obj);
  CPPUNIT_TEST(test_methods_of_arg_obj);
  CPPUNIT_TEST(test_push_and_pop_one_scope);
  CPPUNIT_TEST(test_push_and_pop_two_scopes);
  CPPUNIT_TEST_SUITE_END();
};

#endif
