//
// testdotat.cpp - .@ Langauge
// by £ukasz Szpakowski
//

#include <cppunit/ui/text/TestRunner.h>
#include "testdotat.hpp"

using namespace std;

//
// TestRefPtr
//

TestRefPtr::MyObj::~MyObj()
{
  m_b=true;
}

void TestRefPtr::setUp()
{
}

void TestRefPtr::tearDown()
{
}

void TestRefPtr::test_one_reference()
{
  bool is_dead=false;

  {
    dotat::RefPtr<MyObj> rp(new MyObj(is_dead));

    // object is live
    CPPUNIT_ASSERT_EQUAL(false, is_dead);
  }
  // object is dead
  CPPUNIT_ASSERT_EQUAL(true, is_dead);
}

void TestRefPtr::test_two_references()
{
  bool is_dead=false;

  {
    dotat::RefPtr<MyObj> rp(new MyObj(is_dead));

    {
      dotat::RefPtr<MyObj> rp2(rp);

      // object is live
      CPPUNIT_ASSERT_EQUAL(false, is_dead);
    }
    // object is live
    CPPUNIT_ASSERT_EQUAL(false, is_dead);
  }
  // object is dead
  CPPUNIT_ASSERT_EQUAL(true, is_dead);
}

//
// TestScope
//

void TestScope::setUp()
{
}

void TestScope::tearDown()
{
}

void TestScope::test_define_and_undefine_variables()
{
  dotat::RefPtr<dotat::Scope> prev_scope1(new dotat::Scope);
  dotat::RefPtr<dotat::Scope> prev_scope2(new dotat::Scope);
  dotat::Scope scope;
  dotat::RefPtr<dotat::Expr> expr1(new dotat::VarExpr("var1"));
  dotat::RefPtr<dotat::Expr> expr2(new dotat::VarExpr("var2"));

  // x1 variable
  CPPUNIT_ASSERT(!scope.is_var("x1"));
  // define x1 variable
  scope.def_var("x1", dotat::Var(expr1, prev_scope1));
  // x1 variable
  CPPUNIT_ASSERT(scope.is_var("x1"));
  CPPUNIT_ASSERT_EQUAL(expr1.get(), scope.var("x1").expr().get());
  CPPUNIT_ASSERT_EQUAL(prev_scope1.get(), scope.var("x1").scope().get());
  // x2 variable
  CPPUNIT_ASSERT(!scope.is_var("x2"));
  // define x2 variable
  scope.def_var("x2", dotat::Var(expr2, prev_scope2));
  // x2 variable
  CPPUNIT_ASSERT(scope.is_var("x2"));
  CPPUNIT_ASSERT_EQUAL(expr2.get(), scope.var("x2").expr().get());
  CPPUNIT_ASSERT_EQUAL(prev_scope2.get(), scope.var("x2").scope().get());
  // undefine x1 variable
  scope.undef_var("x1");
  // x1 variable
  CPPUNIT_ASSERT(!scope.is_var("x1"));
  // undefine x2 variable
  scope.undef_var("x2");
  // x2 variable
  CPPUNIT_ASSERT(!scope.is_var("x2"));
}

//
// TestMethod
//

void TestMethod::setUp()
{
  m_interp=dotat::Ptr<dotat::Interp>(new dotat::Interp);
}

void TestMethod::tearDown()
{
  delete m_interp.get();
}

namespace
{
  dotat::Val native_fun1(dotat::Interp &interp, const dotat::RefPtr<dotat::Expr> &arg, const dotat::RefPtr<dotat::Obj> &/*data*/)
  {
    dotat::Val tmp_self=interp.top_scope()->self();

    return dotat::Val(tmp_self.i()+1111, tmp_self.obj());
  }
}

void TestMethod::test_call_native_method()
{
  dotat::Val rcvr(1111, new dotat::Obj);
  dotat::RefPtr<dotat::Expr> arg(new dotat::VarExpr("arg"));
  dotat::Method method(native_fun1);
  dotat::Val r=method.call(*m_interp, rcvr, arg);

  CPPUNIT_ASSERT_EQUAL(2222, r.i());
  CPPUNIT_ASSERT_EQUAL(rcvr.obj().get(), r.obj().get());
}

namespace
{
  dotat::Val native_fun2(dotat::Interp &interp, const dotat::RefPtr<dotat::Expr> &arg, const dotat::RefPtr<dotat::Obj> &data)
  {
    dotat::Val tmp_self=interp.top_scope()->self();

    return dotat::Val(tmp_self.i()+2222, data);
  }
}

void TestMethod::test_call_native_method_with_data()
{
  dotat::Val rcvr1(1111, new dotat::Obj);
  dotat::Val rcvr2(2222, new dotat::Obj);
  dotat::RefPtr<dotat::Obj> data1(new dotat::Obj);
  dotat::RefPtr<dotat::Obj> data2(new dotat::Obj);
  dotat::RefPtr<dotat::Expr> arg(new dotat::VarExpr("arg"));
  dotat::Method method1(native_fun2, data1);
  dotat::Method method2(native_fun2, data2);
  // data 1
  dotat::Val r1=method1.call(*m_interp, rcvr1, arg);

  CPPUNIT_ASSERT_EQUAL(3333, r1.i());
  CPPUNIT_ASSERT_EQUAL(data1.get(), r1.obj().get());
  // data 2
  dotat::Val r2=method2.call(*m_interp, rcvr2, arg);

  CPPUNIT_ASSERT_EQUAL(4444, r2.i());
  CPPUNIT_ASSERT_EQUAL(data2.get(), r2.obj().get());
}

void TestMethod::test_call_method_with_self_expr()
{
  dotat::Val rcvr(1111, new dotat::Obj);
  dotat::RefPtr<dotat::Expr> arg(new dotat::ValExpr(m_interp->num_val(2222)));
  dotat::RefPtr<dotat::Expr> expr(new dotat::SelfExpr);
  dotat::Method method("x", expr);
  dotat::Val r=method.call(*m_interp, rcvr, arg);

  CPPUNIT_ASSERT_EQUAL(1111, r.i());
  CPPUNIT_ASSERT_EQUAL(rcvr.obj().get(), r.obj().get());
}

void TestMethod::test_call_method_with_var_expr()
{
  dotat::Val rcvr(1111, new dotat::Obj);
  dotat::Val arg_val(2222, new dotat::Obj);
  dotat::RefPtr<dotat::Expr> arg(new dotat::ValExpr(arg_val));
  dotat::RefPtr<dotat::Expr> expr(new dotat::VarExpr("x"));
  dotat::Method method("x", expr);
  dotat::Val r=method.call(*m_interp, rcvr, arg);

  CPPUNIT_ASSERT_EQUAL(2222, r.i());
  CPPUNIT_ASSERT_EQUAL(arg_val.obj().get(), r.obj().get());
}

void TestMethod::test_call_currying_method()
{
  dotat::Val rcvr(1111, new dotat::Obj);
  dotat::Val arg_val1(2222, new dotat::Obj);
  dotat::Val arg_val2(3333, new dotat::Obj);
  dotat::RefPtr<dotat::Expr> arg1(new dotat::ValExpr(arg_val1));
  dotat::RefPtr<dotat::Expr> arg2(new dotat::ValExpr(arg_val2));
  dotat::RefPtr<dotat::Expr> expr1(new dotat::VarExpr("x1"));
  dotat::RefPtr<dotat::Expr> expr2(new dotat::VarExpr("x2"));
  dotat::Method method1("x2", expr1);
  dotat::Method method2("x2", expr2);

  // method 1 (used first argument)
  rcvr.obj()->scope().def_var("x1", dotat::Var(arg1, m_interp->top_scope()));
  dotat::Val r1=method1.call(*m_interp, rcvr, arg2);

  CPPUNIT_ASSERT_EQUAL(2222, r1.i());
  CPPUNIT_ASSERT_EQUAL(arg_val1.obj().get(), r1.obj().get());
  // method 2 (used second argument)
  rcvr.obj()->scope().def_var("x1", dotat::Var(arg1, m_interp->top_scope()));
  dotat::Val r2=method2.call(*m_interp, rcvr, arg2);

  CPPUNIT_ASSERT_EQUAL(3333, r2.i());
  CPPUNIT_ASSERT_EQUAL(arg_val2.obj().get(), r2.obj().get());
}

//
// TestObj
//

void TestObj::setUp()
{
}

void TestObj::tearDown()
{
}

void TestObj::test_define_and_undefine_methods()
{
  dotat::RefPtr<dotat::Obj> obj(new dotat::Obj);
  dotat::RefPtr<dotat::Expr> expr1(new dotat::VarExpr("x1"));
  dotat::RefPtr<dotat::Expr> expr2(new dotat::VarExpr("x2"));

  // m1 method
  CPPUNIT_ASSERT(!obj->is_method("m1"));
  // define m1 method
  obj->def_method("m1", dotat::Method("x1", expr1));
  // m1 method
  CPPUNIT_ASSERT(obj->is_method("m1"));
  CPPUNIT_ASSERT_EQUAL(string("x1"), obj->method("m1").arg_name());
  CPPUNIT_ASSERT_EQUAL(expr1.get(), obj->method("m1").expr().get());
  // m2 method
  CPPUNIT_ASSERT(!obj->is_method("m2"));
  // define m2 method
  obj->def_method("m2", dotat::Method("x2", expr2));
  // m2 method
  CPPUNIT_ASSERT(obj->is_method("m2"));
  CPPUNIT_ASSERT_EQUAL(string("x2"), obj->method("m2").arg_name());
  CPPUNIT_ASSERT_EQUAL(expr2.get(), obj->method("m2").expr().get());
  // undefine m1 method
  obj->undef_method("m1");
  // m1 method
  CPPUNIT_ASSERT(!obj->is_method("m1"));
  // undefine m2 method
  obj->undef_method("m2");
  // m2 method
  CPPUNIT_ASSERT(!obj->is_method("m2"));
}

//
// TestExpr
//

void TestExpr::setUp()
{
  m_interp=dotat::Ptr<dotat::Interp>(new dotat::Interp);
  m_oss=dotat::Ptr<ostringstream>(new ostringstream);
}

void TestExpr::tearDown()
{
  delete m_oss.get();
  delete m_interp.get();
}

void TestExpr::test_evaluate_val_expr()
{
  dotat::RefPtr<dotat::Expr> expr(new dotat::ValExpr(m_interp->num_val(1111)));
  dotat::Val val=expr->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(1111, val.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), val.obj().get());
}

void TestExpr::test_evaluate_var_expr()
{
  dotat::RefPtr<dotat::Scope> scope(new dotat::Scope);
  dotat::RefPtr<dotat::Expr> subexpr1(new dotat::ValExpr(m_interp->num_val(1111)));
  dotat::RefPtr<dotat::Expr> subexpr2(new dotat::ValExpr(m_interp->num_val(2222)));

  scope->def_var("x1", dotat::Var(subexpr1, m_interp->top_scope()));
  scope->def_var("x2", dotat::Var(subexpr2, m_interp->top_scope()));
  m_interp->push_scope(scope);
  dotat::RefPtr<dotat::Expr> expr1(new dotat::VarExpr("x1"));
  dotat::RefPtr<dotat::Expr> expr2(new dotat::VarExpr("x2"));
  dotat::RefPtr<dotat::Expr> expr3(new dotat::VarExpr("x3"));
  dotat::Val val1=expr1->eval(*m_interp);
  dotat::Val val2=expr2->eval(*m_interp);
  dotat::Val val3=expr3->eval(*m_interp);

  // expr 1
  CPPUNIT_ASSERT_EQUAL(1111, val1.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), val1.obj().get());
  // expr 2
  CPPUNIT_ASSERT_EQUAL(2222, val2.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), val2.obj().get());
  // expr 3
  CPPUNIT_ASSERT_EQUAL(0, val3.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->nil_obj().get(), val3.obj().get());
  m_interp->pop_scope();
}

void TestExpr::test_evaluate_self_expr()
{
  dotat::RefPtr<dotat::Scope> scope(new dotat::Scope);

  scope->set_self(m_interp->num_val(1111));
  m_interp->push_scope(scope);
  dotat::RefPtr<dotat::Expr> expr(new dotat::SelfExpr);
  dotat::Val val=expr->eval(*m_interp);

  // self
  CPPUNIT_ASSERT_EQUAL(1111, val.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), val.obj().get());
  m_interp->pop_scope();
}

void TestExpr::test_evaluate_send_method_expr()
{
  dotat::RefPtr<dotat::Expr> rcvr(new dotat::ValExpr(m_interp->num_val(3333)));
  dotat::RefPtr<dotat::Expr> arg(new dotat::ValExpr(m_interp->num_val(1111)));
  dotat::RefPtr<dotat::Expr> expr1(new dotat::SendMethodExpr(rcvr, "-", arg));
  dotat::RefPtr<dotat::Expr> expr2(new dotat::SendMethodExpr(rcvr, "/", arg));
  dotat::Val val1=expr1->eval(*m_interp);
  dotat::Val val2=expr2->eval(*m_interp);

  // send method expr 1
  CPPUNIT_ASSERT_EQUAL(2222, val1.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), val1.obj().get());
  // send method expr 2
  CPPUNIT_ASSERT_EQUAL(3, val2.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), val2.obj().get());
}

void TestExpr::test_define_and_send_method_1_arg()
{
  // 0.d(m).a(x).e(x.-(1111))
  dotat::RefPtr<dotat::Expr> d0(new dotat::ValExpr(m_interp->num_val(0)));
  dotat::RefPtr<dotat::Expr> d_m(new dotat::VarExpr("m"));
  dotat::RefPtr<dotat::Expr> d1(new dotat::SendMethodExpr(d0, "d", d_m));
  dotat::RefPtr<dotat::Expr> d_x(new dotat::VarExpr("x"));
  dotat::RefPtr<dotat::Expr> d2(new dotat::SendMethodExpr(d1, "a", d_x));
  dotat::RefPtr<dotat::Expr> de_x(new dotat::VarExpr("x"));
  dotat::RefPtr<dotat::Expr> de_val(new dotat::ValExpr(m_interp->num_val(1111)));
  dotat::RefPtr<dotat::Expr> d_expr(new dotat::SendMethodExpr(de_x, "-", de_val));
  dotat::RefPtr<dotat::Expr> d(new dotat::SendMethodExpr(d2, "e", d_expr));
  dotat::Val d_r=d->eval(*m_interp);

  CPPUNIT_ASSERT(d_r.obj()->is_method("m"));
  dotat::Method method=d_r.obj()->method("m");

  CPPUNIT_ASSERT_EQUAL(d_expr.get(), method.expr().get());
  // d_r.m(4444)
  dotat::RefPtr<dotat::Expr> e0(new dotat::ValExpr(d_r));
  dotat::RefPtr<dotat::Expr> e_arg(new dotat::ValExpr(m_interp->num_val(4444)));
  dotat::RefPtr<dotat::Expr> e(new dotat::SendMethodExpr(e0, "m", e_arg));
  dotat::Val e_r=e->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(3333, e_r.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), e_r.obj().get());
  // d_r.m(y)
  dotat::RefPtr<dotat::Expr> f0(new dotat::ValExpr(d_r));
  dotat::RefPtr<dotat::Expr> f_arg(new dotat::VarExpr("y"));
  dotat::RefPtr<dotat::Expr> f(new dotat::SendMethodExpr(f0, "m", f_arg));
  dotat::RefPtr<dotat::Expr> y(new dotat::ValExpr(m_interp->num_val(5555)));
  dotat::RefPtr<dotat::Scope> scope(new dotat::Scope);

  scope->def_var("y", dotat::Var(y, m_interp->top_scope()));
  m_interp->push_scope(scope);
  dotat::Val f_r=f->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(4444, f_r.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), f_r.obj().get());
  m_interp->pop_scope();
}

void TestExpr::test_define_and_send_method_2_args()
{
  // 0.d(m).a(x1).a(x2).e(x1.-(x2.*(2)))
  dotat::RefPtr<dotat::Expr> d0(new dotat::ValExpr(m_interp->num_val(0)));
  dotat::RefPtr<dotat::Expr> d_m(new dotat::VarExpr("m"));
  dotat::RefPtr<dotat::Expr> d1(new dotat::SendMethodExpr(d0, "d", d_m));
  dotat::RefPtr<dotat::Expr> d_x1(new dotat::VarExpr("x1"));
  dotat::RefPtr<dotat::Expr> d2(new dotat::SendMethodExpr(d1, "a", d_x1));
  dotat::RefPtr<dotat::Expr> d_x2(new dotat::VarExpr("x2"));
  dotat::RefPtr<dotat::Expr> d3(new dotat::SendMethodExpr(d2, "a", d_x2));
  dotat::RefPtr<dotat::Expr> de_x1(new dotat::VarExpr("x1"));
  dotat::RefPtr<dotat::Expr> de(new dotat::ValExpr(m_interp->num_val(1111)));
  dotat::RefPtr<dotat::Expr> dee_x2(new dotat::VarExpr("x2"));
  dotat::RefPtr<dotat::Expr> dee_val(new dotat::ValExpr(m_interp->num_val(2)));
  dotat::RefPtr<dotat::Expr> de_expr(new dotat::SendMethodExpr(dee_x2, "*", dee_val));
  dotat::RefPtr<dotat::Expr> d_expr(new dotat::SendMethodExpr(de_x1, "-", de_expr));
  dotat::RefPtr<dotat::Expr> d(new dotat::SendMethodExpr(d3, "e", d_expr));
  dotat::Val d_r=d->eval(*m_interp);

  CPPUNIT_ASSERT(d_r.obj()->is_method("m"));
  dotat::Method method1=d_r.obj()->method("m");

  CPPUNIT_ASSERT(method1.data()->is_method("a"));
  dotat::Method method2=method1.data()->method("a");

  CPPUNIT_ASSERT_EQUAL(d_expr.get(), method2.expr().get());
  // d_r.m(4444).a(1111)
  dotat::RefPtr<dotat::Expr> e0(new dotat::ValExpr(d_r));
  dotat::RefPtr<dotat::Expr> e_arg1(new dotat::ValExpr(m_interp->num_val(4444)));
  dotat::RefPtr<dotat::Expr> e1(new dotat::SendMethodExpr(e0, "m", e_arg1));
  dotat::RefPtr<dotat::Expr> e_arg2(new dotat::ValExpr(m_interp->num_val(1111)));
  dotat::RefPtr<dotat::Expr> e(new dotat::SendMethodExpr(e1, "a", e_arg2));
  dotat::Val e_r=e->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(2222, e_r.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), e_r.obj().get());
  // d_r.m(y1).a(y2)
  dotat::RefPtr<dotat::Expr> f0(new dotat::ValExpr(d_r));
  dotat::RefPtr<dotat::Expr> f_arg1(new dotat::VarExpr("y1"));
  dotat::RefPtr<dotat::Expr> f1(new dotat::SendMethodExpr(f0, "m", f_arg1));
  dotat::RefPtr<dotat::Expr> f_arg2(new dotat::VarExpr("y2"));
  dotat::RefPtr<dotat::Expr> f(new dotat::SendMethodExpr(f1, "a", f_arg2));
  dotat::RefPtr<dotat::Expr> y1(new dotat::ValExpr(m_interp->num_val(5555)));
  dotat::RefPtr<dotat::Expr> y2(new dotat::ValExpr(m_interp->num_val(2222)));
  dotat::RefPtr<dotat::Scope> scope(new dotat::Scope);

  scope->def_var("y1", dotat::Var(y1, m_interp->top_scope()));
  scope->def_var("y2", dotat::Var(y2, m_interp->top_scope()));
  m_interp->push_scope(scope);
  dotat::Val f_r=f->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(1111, f_r.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), f_r.obj().get());
  m_interp->pop_scope();
}

void TestExpr::test_divided_currying()
{
  // 0.d(m).a(x1).a(x2).e(x1.-x2)
  dotat::RefPtr<dotat::Expr> a0(new dotat::ValExpr(m_interp->num_val(0)));
  dotat::RefPtr<dotat::Expr> a_m(new dotat::VarExpr("m"));
  dotat::RefPtr<dotat::Expr> a1(new dotat::SendMethodExpr(a0, "d", a_m));
  dotat::RefPtr<dotat::Expr> a_x1(new dotat::VarExpr("x1"));
  dotat::RefPtr<dotat::Expr> a2(new dotat::SendMethodExpr(a1, "a", a_x1));
  dotat::RefPtr<dotat::Expr> a_x2(new dotat::VarExpr("x2"));
  dotat::RefPtr<dotat::Expr> a3(new dotat::SendMethodExpr(a2, "a", a_x2));
  dotat::RefPtr<dotat::Expr> ae_x1(new dotat::VarExpr("x1"));
  dotat::RefPtr<dotat::Expr> ae_x2(new dotat::VarExpr("x2"));
  dotat::RefPtr<dotat::Expr> a_expr(new dotat::SendMethodExpr(ae_x1, "-", ae_x2));
  dotat::RefPtr<dotat::Expr> a(new dotat::SendMethodExpr(a3, "e", a_expr));
  dotat::Val a_r=a->eval(*m_interp);
  // 0.d(m).a(x1).a(x2).e(x1.m(x2.+(1111)))
  dotat::RefPtr<dotat::Expr> b0(new dotat::ValExpr(m_interp->num_val(0)));
  dotat::RefPtr<dotat::Expr> b_m(new dotat::VarExpr("m"));
  dotat::RefPtr<dotat::Expr> b1(new dotat::SendMethodExpr(b0, "d", b_m));
  dotat::RefPtr<dotat::Expr> b_x1(new dotat::VarExpr("x1"));
  dotat::RefPtr<dotat::Expr> b2(new dotat::SendMethodExpr(b1, "a", b_x1));
  dotat::RefPtr<dotat::Expr> b_x2(new dotat::VarExpr("x2"));
  dotat::RefPtr<dotat::Expr> b3(new dotat::SendMethodExpr(b2, "a", b_x2));
  dotat::RefPtr<dotat::Expr> be_x1(new dotat::VarExpr("x1"));
  dotat::RefPtr<dotat::Expr> bee_x2(new dotat::VarExpr("x2"));
  dotat::RefPtr<dotat::Expr> bee_val(new dotat::ValExpr(m_interp->num_val(1111)));
  dotat::RefPtr<dotat::Expr> be_expr(new dotat::SendMethodExpr(bee_x2, "+", bee_val));
  dotat::RefPtr<dotat::Expr> b_expr(new dotat::SendMethodExpr(be_x1, "m", be_expr));
  dotat::RefPtr<dotat::Expr> b(new dotat::SendMethodExpr(b3, "e", b_expr));
  dotat::Val b_r=b->eval(*m_interp);
  // b_r.m(a_r).a(2222).a(1010)
  dotat::RefPtr<dotat::Expr> e0(new dotat::ValExpr(b_r));
  dotat::RefPtr<dotat::Expr> e_arg1(new dotat::ValExpr(a_r));
  dotat::RefPtr<dotat::Expr> e1(new dotat::SendMethodExpr(e0, "m", e_arg1));
  dotat::RefPtr<dotat::Expr> e_arg2(new dotat::ValExpr(m_interp->num_val(2222)));
  dotat::RefPtr<dotat::Expr> e2(new dotat::SendMethodExpr(e1, "a", e_arg2));
  dotat::RefPtr<dotat::Expr> e_arg3(new dotat::ValExpr(m_interp->num_val(1010)));
  dotat::RefPtr<dotat::Expr> e(new dotat::SendMethodExpr(e2, "a", e_arg3));
  dotat::Val e_r=e->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(2323, e_r.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), e_r.obj().get());
}

void TestExpr::test_double_evaluation()
{
  dotat::RefPtr<dotat::Scope> scope1(new dotat::Scope);
  dotat::RefPtr<dotat::Scope> scope2(new dotat::Scope);
  dotat::RefPtr<dotat::Expr> subexpr11(new dotat::ValExpr(m_interp->num_val(1111)));
  dotat::RefPtr<dotat::Expr> subexpr12(new dotat::ValExpr(m_interp->num_val(2222)));
  dotat::RefPtr<dotat::Expr> subexpr21(new dotat::ValExpr(m_interp->num_val(3333)));
  dotat::RefPtr<dotat::Expr> subexpr22(new dotat::ValExpr(m_interp->num_val(1111)));
  dotat::RefPtr<dotat::Expr> rcvr(new dotat::VarExpr("x1"));
  dotat::RefPtr<dotat::Expr> arg(new dotat::VarExpr("x2"));
  dotat::RefPtr<dotat::Expr> expr(new dotat::SendMethodExpr(rcvr, "+", arg));


  // scope 1
  scope1->def_var("x1", dotat::Var(subexpr11, m_interp->top_scope()));
  scope1->def_var("x2", dotat::Var(subexpr12, m_interp->top_scope()));
  m_interp->push_scope(scope1);
  dotat::Val val1=expr->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(3333, val1.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), val1.obj().get());
  m_interp->pop_scope();
  // scope 2
  scope2->def_var("x1", dotat::Var(subexpr21, m_interp->top_scope()));
  scope2->def_var("x2", dotat::Var(subexpr22, m_interp->top_scope()));
  m_interp->push_scope(scope2);
  dotat::Val val2=expr->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(4444, val2.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), val2.obj().get());
  m_interp->pop_scope();
}

void TestExpr::test_double_evaluation_var_rcvr()
{
  dotat::RefPtr<dotat::Scope> scope1(new dotat::Scope);
  dotat::RefPtr<dotat::Scope> scope2(new dotat::Scope);
  dotat::RefPtr<dotat::Expr> subexpr1(new dotat::ValExpr(m_interp->num_val(1111)));
  dotat::RefPtr<dotat::Expr> subexpr2(new dotat::ValExpr(m_interp->num_val(3333)));
  dotat::RefPtr<dotat::Expr> rcvr(new dotat::VarExpr("x"));
  dotat::RefPtr<dotat::Expr> arg(new dotat::ValExpr(m_interp->num_val(2222)));
  dotat::RefPtr<dotat::Expr> expr(new dotat::SendMethodExpr(rcvr, "+", arg));

  // scope 1
  scope1->def_var("x", dotat::Var(subexpr1, m_interp->top_scope()));
  m_interp->push_scope(scope1);
  dotat::Val val1=expr->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(3333, val1.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), val1.obj().get());
  m_interp->pop_scope();
  // scope 2
  scope2->def_var("x", dotat::Var(subexpr2, m_interp->top_scope()));
  m_interp->push_scope(scope2);
  dotat::Val val2=expr->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(5555, val2.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), val2.obj().get());
  m_interp->pop_scope();
}

void TestExpr::test_double_evaluation_var_arg()
{
  dotat::RefPtr<dotat::Scope> scope1(new dotat::Scope);
  dotat::RefPtr<dotat::Scope> scope2(new dotat::Scope);
  dotat::RefPtr<dotat::Expr> subexpr1(new dotat::ValExpr(m_interp->num_val(2222)));
  dotat::RefPtr<dotat::Expr> subexpr2(new dotat::ValExpr(m_interp->num_val(4444)));
  dotat::RefPtr<dotat::Expr> rcvr(new dotat::ValExpr(m_interp->num_val(1111)));
  dotat::RefPtr<dotat::Expr> arg(new dotat::VarExpr("x"));
  dotat::RefPtr<dotat::Expr> expr(new dotat::SendMethodExpr(rcvr, "+", arg));

  // scope 1
  scope1->def_var("x", dotat::Var(subexpr1, m_interp->top_scope()));
  m_interp->push_scope(scope1);
  dotat::Val val1=expr->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(3333, val1.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), val1.obj().get());
  m_interp->pop_scope();
  // scope 2
  scope2->def_var("x", dotat::Var(subexpr2, m_interp->top_scope()));
  m_interp->push_scope(scope2);
  dotat::Val val2=expr->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(5555, val2.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), val2.obj().get());
  m_interp->pop_scope();
}

void TestExpr::test_define_and_send_self_method_1_arg()
{
  // 1111.d(m).a(x).e(@)
  dotat::RefPtr<dotat::Expr> d0(new dotat::ValExpr(m_interp->num_val(1111)));
  dotat::RefPtr<dotat::Expr> d_m(new dotat::VarExpr("m"));
  dotat::RefPtr<dotat::Expr> d1(new dotat::SendMethodExpr(d0, "d", d_m));
  dotat::RefPtr<dotat::Expr> d_x(new dotat::VarExpr("x"));
  dotat::RefPtr<dotat::Expr> d2(new dotat::SendMethodExpr(d1, "a", d_x));
  dotat::RefPtr<dotat::Expr> d_expr(new dotat::SelfExpr());
  dotat::RefPtr<dotat::Expr> d(new dotat::SendMethodExpr(d2, "e", d_expr));
  dotat::Val d_r=d->eval(*m_interp);
  // d_r.m(2222)
  dotat::RefPtr<dotat::Expr> e0(new dotat::ValExpr(d_r));
  dotat::RefPtr<dotat::Expr> e_arg(new dotat::ValExpr(m_interp->num_val(2222)));
  dotat::RefPtr<dotat::Expr> e(new dotat::SendMethodExpr(e0, "m", e_arg));
  dotat::Val e_r=e->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(1111, e_r.i());
  CPPUNIT_ASSERT_EQUAL(d_r.obj().get(), e_r.obj().get());
}

void TestExpr::test_define_and_send_self_method_2_args()
{
  // 1111.d(m).a(x1).a(x2).e(@)
  dotat::RefPtr<dotat::Expr> d0(new dotat::ValExpr(m_interp->num_val(1111)));
  dotat::RefPtr<dotat::Expr> d_m(new dotat::VarExpr("m"));
  dotat::RefPtr<dotat::Expr> d1(new dotat::SendMethodExpr(d0, "d", d_m));
  dotat::RefPtr<dotat::Expr> d_x1(new dotat::VarExpr("x1"));
  dotat::RefPtr<dotat::Expr> d2(new dotat::SendMethodExpr(d1, "a", d_x1));
  dotat::RefPtr<dotat::Expr> d_x2(new dotat::VarExpr("x2"));
  dotat::RefPtr<dotat::Expr> d3(new dotat::SendMethodExpr(d2, "a", d_x2));
  dotat::RefPtr<dotat::Expr> d_expr(new dotat::SelfExpr());
  dotat::RefPtr<dotat::Expr> d(new dotat::SendMethodExpr(d3, "e", d_expr));
  dotat::Val d_r=d->eval(*m_interp);
  // d_r.m(2222).a(4444)
  dotat::RefPtr<dotat::Expr> e0(new dotat::ValExpr(d_r));
  dotat::RefPtr<dotat::Expr> e_arg1(new dotat::ValExpr(m_interp->num_val(2222)));
  dotat::RefPtr<dotat::Expr> e1(new dotat::SendMethodExpr(e0, "m", e_arg1));
  dotat::RefPtr<dotat::Expr> e_arg2(new dotat::ValExpr(m_interp->num_val(4444)));
  dotat::RefPtr<dotat::Expr> e(new dotat::SendMethodExpr(e1, "a", e_arg2));
  dotat::Val e_r=e->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(1111, e_r.i());
  CPPUNIT_ASSERT_EQUAL(d_r.obj().get(), e_r.obj().get());
}

namespace
{
  dotat::Val native_fun3(dotat::Interp &interp, const dotat::RefPtr<dotat::Expr> &arg, const dotat::RefPtr<dotat::Obj> &data)
  {
    *(interp.out()) << "x";
    return interp.num_val(1111);
  }
}

void TestExpr::test_evaluate_determined_method()
{
  m_interp->set_out(m_oss);
  dotat::Val fun_val(0, new dotat::Obj);

  fun_val.obj()->def_method("m", dotat::Method(native_fun3, true));
  dotat::RefPtr<dotat::Expr> rcvr(new dotat::ValExpr(fun_val));
  dotat::RefPtr<dotat::Expr> arg(new dotat::ValExpr(m_interp->num_val(0)));
  dotat::RefPtr<dotat::Expr> expr(new dotat::SendMethodExpr(rcvr, "m", arg));
  dotat::Val val1=expr->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(1111, val1.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), val1.obj().get());
  CPPUNIT_ASSERT_EQUAL(string("x"), m_oss->str());
  dotat::Val val2=expr->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(1111, val2.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), val2.obj().get());
  CPPUNIT_ASSERT_EQUAL(string("x"), m_oss->str());
}

void TestExpr::test_evaluate_undetermined_method()
{
  m_interp->set_out(m_oss);
  dotat::Val fun_val(0, new dotat::Obj);

  fun_val.obj()->def_method("m", dotat::Method(native_fun3, false));
  dotat::RefPtr<dotat::Expr> rcvr(new dotat::ValExpr(fun_val));
  dotat::RefPtr<dotat::Expr> arg(new dotat::ValExpr(m_interp->num_val(0)));
  dotat::RefPtr<dotat::Expr> expr(new dotat::SendMethodExpr(rcvr, "m", arg));
  dotat::Val val1=expr->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(1111, val1.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), val1.obj().get());
  CPPUNIT_ASSERT_EQUAL(string("x"), m_oss->str());
  dotat::Val val2=expr->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(1111, val2.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), val2.obj().get());
  CPPUNIT_ASSERT_EQUAL(string("xx"), m_oss->str());
}

void TestExpr::test_detect_infinite_tail_recursion()
{
  dotat::RefPtr<dotat::Scope> scope(new dotat::Scope);
  dotat::RefPtr<dotat::Expr> rcvr(new dotat::SelfExpr);
  dotat::RefPtr<dotat::Expr> arg(new dotat::ValExpr(m_interp->num_val(1111)));
  dotat::RefPtr<dotat::Expr> expr(new dotat::SendMethodExpr(rcvr, "m", arg));
  dotat::Val self_val(2222, new dotat::Obj);
  dotat::Tail tail;

  self_val.obj()->def_method("m", dotat::Method("x", expr));
  m_interp->push_scope(scope);
  m_interp->top_scope()->set_self(self_val);
  dotat::Val val=expr->eval(*m_interp, &tail);

  CPPUNIT_ASSERT(val.obj().get()==0);
  CPPUNIT_ASSERT_EQUAL(expr.get(), tail.expr.get());
  CPPUNIT_ASSERT(tail.scope.get()!=scope.get());
  CPPUNIT_ASSERT_EQUAL(2222, tail.scope->self().i());
  CPPUNIT_ASSERT_EQUAL(self_val.obj().get(), tail.scope->self().obj().get());
  m_interp->pop_scope();
}

void TestExpr::test_detect_tail_recursion()
{
  dotat::RefPtr<dotat::Scope> scope(new dotat::Scope);
  // x.>(0).|(@.m(x.-(1)))
  dotat::RefPtr<dotat::Expr> e0(new dotat::VarExpr("x"));
  dotat::RefPtr<dotat::Expr> e_0(new dotat::ValExpr(m_interp->num_val(0)));
  dotat::RefPtr<dotat::Expr> e1(new dotat::SendMethodExpr(e0, ">", e_0));
  dotat::RefPtr<dotat::Expr> ea_rcvr(new dotat::SelfExpr);
  dotat::RefPtr<dotat::Expr> eaa_x(new dotat::VarExpr("x"));
  dotat::RefPtr<dotat::Expr> eaa_1(new dotat::ValExpr(m_interp->num_val(1)));
  dotat::RefPtr<dotat::Expr> ea_arg(new dotat::SendMethodExpr(eaa_x, "-", eaa_1));
  dotat::RefPtr<dotat::Expr> e_arg(new dotat::SendMethodExpr(ea_rcvr,"m", ea_arg));
  dotat::RefPtr<dotat::Expr> e(new dotat::SendMethodExpr(e1, "|", e_arg));
  dotat::Val self_val(1111, new dotat::Obj);
  dotat::Tail tail;

  self_val.obj()->def_method("m", dotat::Method("x", e));
  m_interp->push_scope(scope);
  m_interp->top_scope()->set_self(self_val);
  dotat::Val e_r=e->eval(*m_interp, &tail);

  CPPUNIT_ASSERT(e_r.obj().get()==0);
  CPPUNIT_ASSERT_EQUAL(e.get(), tail.expr.get());
  CPPUNIT_ASSERT(tail.scope.get()!=scope.get());
  CPPUNIT_ASSERT_EQUAL(1111, tail.scope->self().i());
  CPPUNIT_ASSERT_EQUAL(self_val.obj().get(), tail.scope->self().obj().get());
  CPPUNIT_ASSERT(tail.scope->is_var("x"));
  CPPUNIT_ASSERT_EQUAL(ea_arg.get(), tail.scope->var("x").expr().get());
  m_interp->pop_scope();
}

void TestExpr::test_substitution()
{
  // 1111.m1(2222).m(x.m2(y))
  dotat::RefPtr<dotat::Expr> ll1(new dotat::ValExpr(m_interp->num_val(1111)));
  dotat::RefPtr<dotat::Expr> lr1(new dotat::ValExpr(m_interp->num_val(2222)));
  dotat::RefPtr<dotat::Expr> l1(new dotat::SendMethodExpr(ll1, "m1", lr1));
  dotat::RefPtr<dotat::Expr> rl1(new dotat::VarExpr("x"));
  dotat::RefPtr<dotat::Expr> rr1(new dotat::VarExpr("y"));
  dotat::RefPtr<dotat::Expr> r1(new dotat::SendMethodExpr(rl1, "m2", rr1));
  dotat::RefPtr<dotat::Expr> e1(new dotat::SendMethodExpr(l1, "m", r1));
  // x -> v1.m(v2)
  dotat::RefPtr<dotat::Expr> l2(new dotat::VarExpr("v1"));
  dotat::RefPtr<dotat::Expr> r2(new dotat::VarExpr("v2"));
  dotat::RefPtr<dotat::Expr> e2(new dotat::SendMethodExpr(l2, "m", r2));
  dotat::RefPtr<dotat::Expr> new_e1=e1->substit("x", e2);

  CPPUNIT_ASSERT(new_e1->left().get()==l1.get());
  CPPUNIT_ASSERT(new_e1->right().get()!=r1.get());
  CPPUNIT_ASSERT(new_e1->right()->left().get()==e2.get());
  CPPUNIT_ASSERT(new_e1->right()->right().get()==rr1.get());
  // y.m1(4444).m(x.m2(y))
  dotat::RefPtr<dotat::Expr> ll3(new dotat::VarExpr("y"));
  dotat::RefPtr<dotat::Expr> lr3(new dotat::ValExpr(m_interp->num_val(444)));
  dotat::RefPtr<dotat::Expr> l3(new dotat::SendMethodExpr(ll3, "m1", lr3));
  dotat::RefPtr<dotat::Expr> rl3(new dotat::VarExpr("x"));
  dotat::RefPtr<dotat::Expr> rr3(new dotat::VarExpr("y"));
  dotat::RefPtr<dotat::Expr> r3(new dotat::SendMethodExpr(rl3, "m2", rr3));
  dotat::RefPtr<dotat::Expr> e3(new dotat::SendMethodExpr(l3, "m", r3));
  // x -> v.+(2)
  dotat::RefPtr<dotat::Expr> l4(new dotat::VarExpr("v"));
  dotat::RefPtr<dotat::Expr> r4(new dotat::ValExpr(m_interp->num_val(2)));
  dotat::RefPtr<dotat::Expr> e4(new dotat::SendMethodExpr(l4, "+", r4));
  dotat::RefPtr<dotat::Expr> new_e3=e3->substit("y", e4);

  CPPUNIT_ASSERT(new_e3->left().get()!=l3.get());
  CPPUNIT_ASSERT(new_e3->left()->left().get()==e4.get());
  CPPUNIT_ASSERT(new_e3->left()->right().get()==lr3.get());
  CPPUNIT_ASSERT(new_e3->right().get()!=r3.get());
  CPPUNIT_ASSERT(new_e3->right()->left().get()==rl3.get());
  CPPUNIT_ASSERT(new_e3->right()->right().get()==e4.get());
}

void TestExpr::test_evaluate_variable_value()
{
  dotat::RefPtr<dotat::Scope> scope(new dotat::Scope);
  dotat::RefPtr<dotat::Expr> se_rcvr(new dotat::ValExpr(m_interp->num_val(1111)));
  dotat::RefPtr<dotat::Expr> se_arg(new dotat::ValExpr(m_interp->num_val(2222)));
  dotat::RefPtr<dotat::Expr> subexpr(new dotat::SendMethodExpr(se_rcvr, "+", se_arg));
  dotat::RefPtr<dotat::Expr> e1_rcvr(new dotat::VarExpr("x"));
  dotat::RefPtr<dotat::Expr> e1_arg(new dotat::ValExpr(m_interp->num_val(2)));
  dotat::RefPtr<dotat::Expr> expr1(new dotat::SendMethodExpr(e1_rcvr, "*", e1_arg));
  dotat::RefPtr<dotat::Expr> e2_rcvr(new dotat::VarExpr("x"));
  dotat::RefPtr<dotat::Expr> e2_arg(new dotat::ValExpr(m_interp->num_val(1010)));
  dotat::RefPtr<dotat::Expr> expr2(new dotat::SendMethodExpr(e2_rcvr, "-", e2_arg));

  scope->def_var("x", dotat::Var(subexpr, m_interp->top_scope()));
  m_interp->push_scope(scope);
  CPPUNIT_ASSERT(m_interp->top_scope()->is_var("x"));
  CPPUNIT_ASSERT(m_interp->top_scope()->var("x").scope().get()!=0);
  // expr 1
  dotat::Val val1=expr1->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(6666, val1.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), val1.obj().get());
  CPPUNIT_ASSERT(m_interp->top_scope()->var("x").scope().get()==0);
  // expr 2
  dotat::Val val2=expr2->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(2323, val2.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), val2.obj().get());
  CPPUNIT_ASSERT(m_interp->top_scope()->var("x").scope().get()==0);
}

void TestExpr::test_can_not_define_method_without_args()
{
  // 0.d(m).e(1111)
  dotat::RefPtr<dotat::Expr> d0(new dotat::ValExpr(m_interp->num_val(0)));
  dotat::RefPtr<dotat::Expr> d_m(new dotat::VarExpr("m"));
  dotat::RefPtr<dotat::Expr> d1(new dotat::SendMethodExpr(d0, "d", d_m));
  dotat::RefPtr<dotat::Expr> d_expr(new dotat::ValExpr(m_interp->num_val(1111)));
  dotat::RefPtr<dotat::Expr> d(new dotat::SendMethodExpr(d1, "e", d_expr));
  dotat::Val d_r=d->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(0, d_r.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->nil_obj().get(), d_r.obj().get());
}

//
// TestParser
//

void TestParser::setUp()
{
  m_interp=dotat::Ptr<dotat::Interp>(new dotat::Interp);
}

void TestParser::tearDown()
{
  delete m_interp.get();
}

void TestParser::test_parse_val_expr()
{
  istringstream iss1("12345");
  dotat::Parser parser1(iss1);
  dotat::RefPtr<dotat::Expr> expr1=parser1.parse(*m_interp);
  dotat::ValExpr *exprp1=dynamic_cast<dotat::ValExpr *>(expr1.get());

  CPPUNIT_ASSERT(exprp1!=0);
  CPPUNIT_ASSERT_EQUAL(12345, exprp1->val().i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), exprp1->val().obj().get());
  istringstream iss2("-56780");
  dotat::Parser parser2(iss2);
  dotat::RefPtr<dotat::Expr> expr2=parser2.parse(*m_interp);
  dotat::ValExpr *exprp2=dynamic_cast<dotat::ValExpr *>(expr2.get());

  CPPUNIT_ASSERT(exprp2!=0);
  CPPUNIT_ASSERT_EQUAL(-56780, exprp2->val().i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), exprp2->val().obj().get());
}

void TestParser::test_parse_var_expr()
{
  istringstream iss1("var21");
  dotat::Parser parser1(iss1);
  dotat::RefPtr<dotat::Expr> expr1=parser1.parse(*m_interp);
  dotat::VarExpr *exprp1=dynamic_cast<dotat::VarExpr *>(expr1.get());

  CPPUNIT_ASSERT(exprp1!=0);
  CPPUNIT_ASSERT_EQUAL(string("var21"), exprp1->var_name());
  istringstream iss2(" \\134a\\.");
  dotat::Parser parser2(iss2);
  dotat::RefPtr<dotat::Expr> expr2=parser2.parse(*m_interp);
  dotat::VarExpr *exprp2=dynamic_cast<dotat::VarExpr *>(expr2.get());

  CPPUNIT_ASSERT(exprp2!=0);
  CPPUNIT_ASSERT_EQUAL(string("134a."), exprp2->var_name());
}

void TestParser::test_parse_self_expr()
{
  istringstream iss("@");
  dotat::Parser parser(iss);
  dotat::RefPtr<dotat::Expr> expr=parser.parse(*m_interp);
  dotat::SelfExpr *exprp=dynamic_cast<dotat::SelfExpr *>(expr.get());

  CPPUNIT_ASSERT(exprp!=0);
}

void TestParser::test_parse_send_method_expr()
{
  // 123 . method(xyz )
  istringstream iss1("123 . method(xyz )");
  dotat::Parser parser1(iss1);
  dotat::RefPtr<dotat::Expr> expr1=parser1.parse(*m_interp);
  dotat::SendMethodExpr *exprp1=dynamic_cast<dotat::SendMethodExpr *>(expr1.get());

  CPPUNIT_ASSERT(exprp1!=0);
  CPPUNIT_ASSERT_EQUAL(string("method"), exprp1->method_name());
  dotat::ValExpr *rcvrp1=dynamic_cast<dotat::ValExpr *>(exprp1->rcvr().get());
  dotat::VarExpr *argp1=dynamic_cast<dotat::VarExpr *>(exprp1->arg().get());

  CPPUNIT_ASSERT(rcvrp1!=0);
  CPPUNIT_ASSERT_EQUAL(123, rcvrp1->val().i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), rcvrp1->val().obj().get());
  CPPUNIT_ASSERT(argp1!=0);
  CPPUNIT_ASSERT_EQUAL(string("xyz"), argp1->var_name());
  // xy.@1(@)
  istringstream iss2("xy.\\@1(@)");
  dotat::Parser parser2(iss2);
  dotat::RefPtr<dotat::Expr> expr2=parser2.parse(*m_interp);
  dotat::SendMethodExpr *exprp2=dynamic_cast<dotat::SendMethodExpr *>(expr2.get());

  CPPUNIT_ASSERT(exprp2!=0);
  CPPUNIT_ASSERT_EQUAL(string("@1"), exprp2->method_name());
  dotat::VarExpr *rcvrp2=dynamic_cast<dotat::VarExpr *>(exprp2->rcvr().get());
  dotat::SelfExpr *argp2=dynamic_cast<dotat::SelfExpr *>(exprp2->arg().get());

  CPPUNIT_ASSERT(rcvrp2!=0);
  CPPUNIT_ASSERT_EQUAL(string("xy"), rcvrp2->var_name());
  CPPUNIT_ASSERT(argp2!=0);
}

void TestParser::test_parse_expr_chain()
{
  // 1.m1(x1).m2(x2).m3(x3)
  istringstream iss("1.m1(x1).m2(x2).m3(x3)");
  dotat::Parser parser(iss);
  dotat::RefPtr<dotat::Expr> expr=parser.parse(*m_interp);
  dotat::SendMethodExpr *exprp=dynamic_cast<dotat::SendMethodExpr *>(expr.get());

  // ... m3(x3)
  CPPUNIT_ASSERT(exprp!=0);
  CPPUNIT_ASSERT_EQUAL(string("m3"), exprp->method_name());
  dotat::SendMethodExpr *rcvrp1=dynamic_cast<dotat::SendMethodExpr *>(exprp->rcvr().get());
  dotat::VarExpr *argp1=dynamic_cast<dotat::VarExpr *>(exprp->arg().get());

  CPPUNIT_ASSERT(argp1!=0);
  CPPUNIT_ASSERT_EQUAL(string("x3"), argp1->var_name());
  // ... m2(x2) ...
  CPPUNIT_ASSERT(rcvrp1!=0);
  CPPUNIT_ASSERT_EQUAL(string("m2"), rcvrp1->method_name());
  dotat::SendMethodExpr *rcvrp2=dynamic_cast<dotat::SendMethodExpr *>(rcvrp1->rcvr().get());
  dotat::VarExpr *argp2=dynamic_cast<dotat::VarExpr *>(rcvrp1->arg().get());

  CPPUNIT_ASSERT(argp2!=0);
  CPPUNIT_ASSERT_EQUAL(string("x2"), argp2->var_name());
  // 1.m1(x1) ...
  CPPUNIT_ASSERT(rcvrp2!=0);
  CPPUNIT_ASSERT_EQUAL(string("m1"), rcvrp2->method_name());
  dotat::ValExpr *rcvrp3=dynamic_cast<dotat::ValExpr *>(rcvrp2->rcvr().get());
  dotat::VarExpr *argp3=dynamic_cast<dotat::VarExpr *>(rcvrp2->arg().get());

  CPPUNIT_ASSERT(argp3!=0);
  CPPUNIT_ASSERT_EQUAL(string("x1"), argp3->var_name());
  CPPUNIT_ASSERT(rcvrp3!=0);
  CPPUNIT_ASSERT_EQUAL(1, rcvrp3->val().i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), rcvrp3->val().obj().get());
}

void TestParser::test_parse_expr_with_subexprs()
{
  // 1.mx(x.nx(456.+(@)).+(4))
  istringstream iss("1.mx(x.nx(456.+(@)).+(4))");
  dotat::Parser parser(iss);
  dotat::RefPtr<dotat::Expr> expr=parser.parse(*m_interp);
  dotat::SendMethodExpr *exprp=dynamic_cast<dotat::SendMethodExpr *>(expr.get());

  CPPUNIT_ASSERT(exprp!=0);
  CPPUNIT_ASSERT_EQUAL(string("mx"), exprp->method_name());
  dotat::ValExpr *rcvrp1=dynamic_cast<dotat::ValExpr *>(exprp->rcvr().get());
  dotat::SendMethodExpr *argp1=dynamic_cast<dotat::SendMethodExpr *>(exprp->arg().get());

  CPPUNIT_ASSERT(rcvrp1!=0);
  CPPUNIT_ASSERT_EQUAL(1, rcvrp1->val().i());
  // ... x.nx(456.+(@)).+(4) ...
  CPPUNIT_ASSERT(argp1!=0);
  CPPUNIT_ASSERT_EQUAL(string("+"), argp1->method_name());
  dotat::SendMethodExpr *rcvrp2=dynamic_cast<dotat::SendMethodExpr *>(argp1->rcvr().get());
  dotat::ValExpr *argp2=dynamic_cast<dotat::ValExpr *>(argp1->arg().get());

  CPPUNIT_ASSERT(argp2!=0);
  CPPUNIT_ASSERT_EQUAL(4, argp2->val().i());
  // ... x.nx(456.+(@)) ...
  CPPUNIT_ASSERT(rcvrp2!=0);
  CPPUNIT_ASSERT_EQUAL(string("nx"), rcvrp2->method_name());
  dotat::VarExpr *rcvrp3=dynamic_cast<dotat::VarExpr *>(rcvrp2->rcvr().get());
  dotat::SendMethodExpr *argp3=dynamic_cast<dotat::SendMethodExpr *>(rcvrp2->arg().get());

  CPPUNIT_ASSERT(rcvrp3!=0);
  CPPUNIT_ASSERT_EQUAL(string("x"), rcvrp3->var_name());
  // ... 456.+(@) ...
  CPPUNIT_ASSERT(argp3!=0);
  CPPUNIT_ASSERT_EQUAL(string("+"), argp3->method_name());
  dotat::ValExpr *rcvrp4=dynamic_cast<dotat::ValExpr *>(argp3->rcvr().get());
  dotat::SelfExpr *argp4=dynamic_cast<dotat::SelfExpr *>(argp3->arg().get());

  CPPUNIT_ASSERT(rcvrp4!=0);
  CPPUNIT_ASSERT_EQUAL(456, rcvrp4->val().i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), rcvrp4->val().obj().get());
  CPPUNIT_ASSERT(argp4!=0);
}

void TestParser::test_skip_comment()
{
  // .@ jjnj@nj\nbhbhuuu@@@.123.@gvgvgvg@.
  istringstream iss1(".@ jjnj@nj\nbhbhuuu@@@.123.@gvgvgvg@.");
  dotat::Parser parser1(iss1);
  dotat::RefPtr<dotat::Expr> expr1=parser1.parse(*m_interp);
  dotat::ValExpr *exprp1=dynamic_cast<dotat::ValExpr *>(expr1.get());

  CPPUNIT_ASSERT(exprp1!=0);
  CPPUNIT_ASSERT_EQUAL(123, exprp1->val().i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), exprp1->val().obj().get());
  // 123.@njnjnj@...@@@@@.m.@...@.(x)
  istringstream iss2("123.@njnjnj@...@@@@@.m.@...@.(x)");
  dotat::Parser parser2(iss2);
  dotat::RefPtr<dotat::Expr> expr2=parser2.parse(*m_interp);
  dotat::SendMethodExpr *exprp2=dynamic_cast<dotat::SendMethodExpr *>(expr2.get());

  CPPUNIT_ASSERT(exprp2!=0);
  CPPUNIT_ASSERT_EQUAL(string("m"), exprp2->method_name());
  dotat::ValExpr *rcvrp2=dynamic_cast<dotat::ValExpr *>(exprp2->rcvr().get());
  dotat::VarExpr *argp2=dynamic_cast<dotat::VarExpr *>(exprp2->arg().get());

  CPPUNIT_ASSERT(rcvrp2!=0);
  CPPUNIT_ASSERT_EQUAL(123, rcvrp2->val().i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), rcvrp2->val().obj().get());
  CPPUNIT_ASSERT(argp2!=0);
  CPPUNIT_ASSERT_EQUAL(string("x"), argp2->var_name());
}

void TestParser::test_empty_method_name()
{
  // @.(x)
  istringstream iss("@.(x)");
  dotat::Parser parser(iss);
  dotat::RefPtr<dotat::Expr> expr=parser.parse(*m_interp);
  dotat::SendMethodExpr *exprp=dynamic_cast<dotat::SendMethodExpr *>(expr.get());

  CPPUNIT_ASSERT(exprp!=0);
  CPPUNIT_ASSERT_EQUAL(string(""), exprp->method_name());
}

void TestParser::test_empty_var_name()
{
  //
  istringstream iss1("");
  dotat::Parser parser1(iss1);
  dotat::RefPtr<dotat::Expr> expr1=parser1.parse(*m_interp);
  dotat::VarExpr *exprp1=dynamic_cast<dotat::VarExpr *>(expr1.get());

  CPPUNIT_ASSERT(exprp1!=0);
  CPPUNIT_ASSERT_EQUAL(string(""), exprp1->var_name());

  //
  istringstream iss2("x.m()");
  dotat::Parser parser2(iss2);
  dotat::RefPtr<dotat::Expr> expr2=parser2.parse(*m_interp);
  dotat::SendMethodExpr *exprp2=dynamic_cast<dotat::SendMethodExpr *>(expr2.get());

  CPPUNIT_ASSERT(exprp2!=0);
  CPPUNIT_ASSERT_EQUAL(string("m"), exprp2->method_name());
  dotat::VarExpr *argp2=dynamic_cast<dotat::VarExpr *>(exprp2->arg().get());

  CPPUNIT_ASSERT(argp2!=0);
  CPPUNIT_ASSERT_EQUAL(string(""), argp2->var_name());
}

void TestParser::test_parse_error()
{
  bool is_except;
  // 0.m
  istringstream iss1("\n\n0.m");
  dotat::Parser parser1(iss1);

  try {
    is_except=false;
    parser1.parse(*m_interp);
  } catch(dotat::ParseError &e) {
    is_except=true;
    CPPUNIT_ASSERT_EQUAL(string("3:"), string(e.what()).substr(0, 2));
  }
  CPPUNIT_ASSERT(is_except);
  // 0.m(x
  istringstream iss2("0.m(x\n");
  dotat::Parser parser2(iss2);

  try {
    is_except=false;
    parser2.parse(*m_interp);
  } catch(dotat::ParseError &e) {
    is_except=true;
    CPPUNIT_ASSERT_EQUAL(string("2:"), string(e.what()).substr(0, 2));
  }
  CPPUNIT_ASSERT(is_except);
  // 0.m(x)m
  istringstream iss3("0.m(x)m\n");
  dotat::Parser parser3(iss3);

  try {
    is_except=false;
    parser3.parse(*m_interp);
  } catch(dotat::ParseError &e) {
    is_except=true;
    CPPUNIT_ASSERT_EQUAL(string("1:"), string(e.what()).substr(0, 2));
  }
  CPPUNIT_ASSERT(is_except);
}

//
// TestInterp
//

void TestInterp::setUp()
{
  m_interp=dotat::Ptr<dotat::Interp>(new dotat::Interp);
}

void TestInterp::tearDown()
{
  delete m_interp.get();
}

void TestInterp::test_methods_of_nil_obj()
{
  // "~" method
  CPPUNIT_ASSERT(m_interp->nil_obj()->is_method("~"));
  // "&" method
  CPPUNIT_ASSERT(m_interp->nil_obj()->is_method("&"));
  // "|" method
  CPPUNIT_ASSERT(m_interp->nil_obj()->is_method("|"));
  // d method
  CPPUNIT_ASSERT(m_interp->nil_obj()->is_method("d"));
  // u method
  CPPUNIT_ASSERT(m_interp->nil_obj()->is_method("u"));
}

void TestInterp::test_methods_of_num_obj()
{
  // "+" method
  CPPUNIT_ASSERT(m_interp->num_obj()->is_method("+"));
  // "-" method
  CPPUNIT_ASSERT(m_interp->num_obj()->is_method("-"));
  // "*" method
  CPPUNIT_ASSERT(m_interp->num_obj()->is_method("*"));
  // "/" method
  CPPUNIT_ASSERT(m_interp->num_obj()->is_method("/"));
  // "%" method
  CPPUNIT_ASSERT(m_interp->num_obj()->is_method("%"));
  // "=" method
  CPPUNIT_ASSERT(m_interp->num_obj()->is_method("="));
  // "<>" method
  CPPUNIT_ASSERT(m_interp->num_obj()->is_method("<>"));
  // "<" method
  CPPUNIT_ASSERT(m_interp->num_obj()->is_method("<"));
  // "<=" method
  CPPUNIT_ASSERT(m_interp->num_obj()->is_method("<="));
  // ">" method
  CPPUNIT_ASSERT(m_interp->num_obj()->is_method(">"));
  // ">=" method
  CPPUNIT_ASSERT(m_interp->num_obj()->is_method(">="));
  // "~" method
  CPPUNIT_ASSERT(m_interp->num_obj()->is_method("~"));
  // "&" method
  CPPUNIT_ASSERT(m_interp->num_obj()->is_method("&"));
  // "|" method
  CPPUNIT_ASSERT(m_interp->num_obj()->is_method("|"));
  // d method
  CPPUNIT_ASSERT(m_interp->num_obj()->is_method("d"));
  // u method
  CPPUNIT_ASSERT(m_interp->num_obj()->is_method("u"));
}

void TestInterp::test_methods_of_method_obj()
{
  // "~" method
  CPPUNIT_ASSERT(m_interp->method_obj()->is_method("~"));
  // "&" method
  CPPUNIT_ASSERT(m_interp->method_obj()->is_method("&"));
  // "|" method
  CPPUNIT_ASSERT(m_interp->method_obj()->is_method("|"));
  // d method
  CPPUNIT_ASSERT(m_interp->method_obj()->is_method("d"));
  // u method
  CPPUNIT_ASSERT(m_interp->method_obj()->is_method("u"));
  // a method
  CPPUNIT_ASSERT(m_interp->method_obj()->is_method("a"));
  // e method
  CPPUNIT_ASSERT(m_interp->method_obj()->is_method("e"));
}

void TestInterp::test_methods_of_arg_obj()
{
  // "~" method
  CPPUNIT_ASSERT(m_interp->arg_obj()->is_method("~"));
  // "&" method
  CPPUNIT_ASSERT(m_interp->arg_obj()->is_method("&"));
  // "|" method
  CPPUNIT_ASSERT(m_interp->arg_obj()->is_method("|"));
  // d method
  CPPUNIT_ASSERT(m_interp->arg_obj()->is_method("d"));
  // u method
  CPPUNIT_ASSERT(m_interp->arg_obj()->is_method("u"));
  // a method
  CPPUNIT_ASSERT(!m_interp->arg_obj()->is_method("a"));
}

void TestInterp::test_push_and_pop_one_scope()
{
  dotat::RefPtr<dotat::Scope> scope(new dotat::Scope);
  dotat::RefPtr<dotat::Expr> expr(new dotat::VarExpr("var"));

  CPPUNIT_ASSERT_EQUAL(1U, m_interp->scopes_count());
  {
    scope->def_var("x", dotat::Var(expr, 0));
    // push scope to stack
    m_interp->push_scope(scope);
    CPPUNIT_ASSERT_EQUAL(2U, m_interp->scopes_count());
    // pop scope from stack
    dotat::RefPtr<dotat::Scope> new_scope=m_interp->top_scope();

    m_interp->pop_scope();
    CPPUNIT_ASSERT_EQUAL(scope.get(), new_scope.get());
  }
  CPPUNIT_ASSERT_EQUAL(1U, m_interp->scopes_count());
}

void TestInterp::test_push_and_pop_two_scopes()
{
  dotat::RefPtr<dotat::Scope> scope1(new dotat::Scope);
  dotat::RefPtr<dotat::Scope> scope2(new dotat::Scope);
  dotat::RefPtr<dotat::Expr> expr1(new dotat::VarExpr("var1"));
  dotat::RefPtr<dotat::Expr> expr2(new dotat::VarExpr("var2"));

  CPPUNIT_ASSERT_EQUAL(1U, m_interp->scopes_count());
  {
    scope1->def_var("x1", dotat::Var(expr1, m_interp->top_scope()));
    // push scope to stack
    m_interp->push_scope(scope1);
    CPPUNIT_ASSERT_EQUAL(2U, m_interp->scopes_count());
    {
      scope2->def_var("x2", dotat::Var(expr2, m_interp->top_scope()));
      // push scope to stack
      m_interp->push_scope(scope2);
      CPPUNIT_ASSERT_EQUAL(3U, m_interp->scopes_count());
      // pop scope from stack
      dotat::RefPtr<dotat::Scope> new_scope2=m_interp->top_scope();

      m_interp->pop_scope();
      CPPUNIT_ASSERT_EQUAL(scope2.get(), new_scope2.get());
    }
    // pop scope from stack
    dotat::RefPtr<dotat::Scope> new_scope1=m_interp->top_scope();

    m_interp->pop_scope();
    CPPUNIT_ASSERT_EQUAL(scope1.get(), new_scope1.get());
  }
  CPPUNIT_ASSERT_EQUAL(1U, m_interp->scopes_count());
}

void TestInterp::test_extract_tree_from_method()
{
  istringstream iss("0.d(m1).a(x).e(x.m2(1)).t(m1)");
  dotat::RefPtr<dotat::Expr> expr(m_interp->parse(iss));
  dotat::Val val=expr->eval(*m_interp);

  CPPUNIT_ASSERT(val.obj()->is_method("m"));
  dotat::RefPtr<dotat::Expr> tree=val.obj()->method("m").expr();
  dotat::SendMethodExpr *treep=dynamic_cast<dotat::SendMethodExpr *>(tree.get());

  CPPUNIT_ASSERT(treep!=0);
  CPPUNIT_ASSERT_EQUAL(string("m2"), treep->method_name());
  dotat::RefPtr<dotat::Expr> left=tree->left();
  dotat::VarExpr *leftp=dynamic_cast<dotat::VarExpr *>(left.get());

  CPPUNIT_ASSERT(leftp!=0);
  CPPUNIT_ASSERT_EQUAL(string("x"), leftp->var_name());
  dotat::RefPtr<dotat::Expr> right=tree->right();
  dotat::ValExpr *rightp=dynamic_cast<dotat::ValExpr *>(right.get());

  CPPUNIT_ASSERT(rightp!=0);
  CPPUNIT_ASSERT_EQUAL(1, rightp->val().i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), rightp->val().obj().get());
}

void TestInterp::test_can_not_extract_tree_from_method_without_expr()
{
  istringstream iss("0.t(+)");
  dotat::RefPtr<dotat::Expr> expr(m_interp->parse(iss));
  dotat::Val val=expr->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(0, val.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->nil_obj().get(), val.obj().get());
}

void TestInterp::test_expr_l()
{
  istringstream iss1("0.d(m1).a(x).e(l1.m2(r1)).t(m1).l(0)");
  dotat::RefPtr<dotat::Expr> expr1(m_interp->parse(iss1));
  dotat::Val val1=expr1->eval(*m_interp);

  CPPUNIT_ASSERT(val1.obj()->is_method("m"));
  dotat::RefPtr<dotat::Expr> tree1=val1.obj()->method("m").expr();
  dotat::VarExpr *treep1=dynamic_cast<dotat::VarExpr *>(tree1.get());

  CPPUNIT_ASSERT(treep1!=0);
  CPPUNIT_ASSERT_EQUAL(string("l1"), treep1->var_name());
  istringstream iss2("0.d(m1).a(x).e(1).t(m1).l(0)");
  dotat::RefPtr<dotat::Expr> expr2(m_interp->parse(iss2));
  dotat::Val val2=expr2->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(m_interp->nil_obj().get(), val2.obj().get());
}

void TestInterp::test_expr_r()
{
  istringstream iss1("0.d(m1).a(x).e(l1.m2(r1)).t(m1).r(0)");
  dotat::RefPtr<dotat::Expr> expr1(m_interp->parse(iss1));
  dotat::Val val1=expr1->eval(*m_interp);

  CPPUNIT_ASSERT(val1.obj()->is_method("m"));
  dotat::RefPtr<dotat::Expr> tree1=val1.obj()->method("m").expr();
  dotat::VarExpr *treep1=dynamic_cast<dotat::VarExpr *>(tree1.get());

  CPPUNIT_ASSERT(treep1!=0);
  CPPUNIT_ASSERT_EQUAL(string("r1"), treep1->var_name());
  istringstream iss2("0.d(m1).a(x).e(1).t(m1).r(0)");
  dotat::RefPtr<dotat::Expr> expr2(m_interp->parse(iss2));
  dotat::Val val2=expr2->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(m_interp->nil_obj().get(), val2.obj().get());
}

void TestInterp::test_expr_l_and_r_for_method_without_expr()
{
  istringstream fun_iss("0.d(m1).a(x).e(x.m2(1)).t(m1)");
  dotat::RefPtr<dotat::Expr> fun_expr(m_interp->parse(fun_iss));
  dotat::Val fun_val=fun_expr->eval(*m_interp);

  fun_val.obj()->def_method("m", dotat::Method(native_fun1, true));
  dotat::RefPtr<dotat::Expr> rcvr(new dotat::ValExpr(fun_val));
  dotat::RefPtr<dotat::Expr> arg(new dotat::ValExpr(m_interp->num_val(0)));
  dotat::RefPtr<dotat::Expr> l_expr(new dotat::SendMethodExpr(rcvr, "l", arg));
  dotat::RefPtr<dotat::Expr> r_expr(new dotat::SendMethodExpr(rcvr, "r", arg));
  dotat::Val l_val=l_expr->eval(*m_interp);
  dotat::Val r_val=r_expr->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(m_interp->nil_obj().get(), l_val.obj().get());
  CPPUNIT_ASSERT_EQUAL(m_interp->nil_obj().get(), r_val.obj().get());
}

void TestInterp::test_expr_sl()
{
  istringstream iss1("0.d(m1).a(x).e(l1.m2(r1)).t(m1).sl(0.d(m).a(x).e(l2))");
  dotat::RefPtr<dotat::Expr> expr1(m_interp->parse(iss1));
  dotat::Val val1=expr1->eval(*m_interp);

  CPPUNIT_ASSERT(val1.obj()->is_method("m"));
  dotat::RefPtr<dotat::Expr> tree1=val1.obj()->method("m").expr();
  dotat::SendMethodExpr *treep1=dynamic_cast<dotat::SendMethodExpr *>(tree1.get());

  CPPUNIT_ASSERT(treep1!=0);
  CPPUNIT_ASSERT_EQUAL(string("m2"), treep1->method_name());
  dotat::RefPtr<dotat::Expr> left1=tree1->left();
  dotat::VarExpr *leftp1=dynamic_cast<dotat::VarExpr *>(left1.get());

  CPPUNIT_ASSERT(leftp1!=0);
  CPPUNIT_ASSERT_EQUAL(string("l2"), leftp1->var_name());
  dotat::RefPtr<dotat::Expr> right1=tree1->right();
  dotat::VarExpr *rightp1=dynamic_cast<dotat::VarExpr *>(right1.get());

  CPPUNIT_ASSERT(rightp1!=0);
  CPPUNIT_ASSERT_EQUAL(string("r1"), rightp1->var_name());
  istringstream iss2("0.d(m1).a(x).e(x).t(m1).sl(0.d(m).a(x).e(l2))");
  dotat::RefPtr<dotat::Expr> expr2(m_interp->parse(iss2));
  dotat::Val val2=expr2->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(m_interp->nil_obj().get(), val2.obj().get());
}

void TestInterp::test_expr_sr()
{
  istringstream iss1("0.d(m1).a(x).e(l1.m2(r1)).t(m1).sr(0.d(m).a(x).e(r2))");
  dotat::RefPtr<dotat::Expr> expr1(m_interp->parse(iss1));
  dotat::Val val1=expr1->eval(*m_interp);

  CPPUNIT_ASSERT(val1.obj()->is_method("m"));
  dotat::RefPtr<dotat::Expr> tree1=val1.obj()->method("m").expr();
  dotat::SendMethodExpr *treep1=dynamic_cast<dotat::SendMethodExpr *>(tree1.get());

  CPPUNIT_ASSERT(treep1!=0);
  CPPUNIT_ASSERT_EQUAL(string("m2"), treep1->method_name());
  dotat::RefPtr<dotat::Expr> left1=tree1->left();
  dotat::VarExpr *leftp1=dynamic_cast<dotat::VarExpr *>(left1.get());

  CPPUNIT_ASSERT(leftp1!=0);
  CPPUNIT_ASSERT_EQUAL(string("l1"), leftp1->var_name());
  dotat::RefPtr<dotat::Expr> right1=tree1->right();
  dotat::VarExpr *rightp1=dynamic_cast<dotat::VarExpr *>(right1.get());

  CPPUNIT_ASSERT(rightp1!=0);
  CPPUNIT_ASSERT_EQUAL(string("r2"), rightp1->var_name());
  istringstream iss2("0.d(m1).a(x).e(x).t(m1).sr(0.d(m).a(x).e(l2))");
  dotat::RefPtr<dotat::Expr> expr2(m_interp->parse(iss2));
  dotat::Val val2=expr2->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(m_interp->nil_obj().get(), val2.obj().get());
}

void TestInterp::test_expr_s()
{
  istringstream iss1("0.d(m1).a(x).e(x).t(m1).s(x).a(0.d(m).a(y).e(y))");
  dotat::RefPtr<dotat::Expr> expr1(m_interp->parse(iss1));
  dotat::Val val1=expr1->eval(*m_interp);

  CPPUNIT_ASSERT(val1.obj()->is_method("m"));
  dotat::RefPtr<dotat::Expr> tree1=val1.obj()->method("m").expr();
  dotat::VarExpr *treep1=dynamic_cast<dotat::VarExpr *>(tree1.get());

  CPPUNIT_ASSERT(treep1!=0);
  CPPUNIT_ASSERT_EQUAL(string("y"), treep1->var_name());
  istringstream iss2("0.d(m1).a(x).e(1111).t(m1).s(x).a(0.d(m).a(y).e(y))");
  dotat::RefPtr<dotat::Expr> expr2(m_interp->parse(iss2));
  dotat::Val val2=expr2->eval(*m_interp);

  CPPUNIT_ASSERT(val2.obj()->is_method("m"));
  dotat::RefPtr<dotat::Expr> tree2=val2.obj()->method("m").expr();
  dotat::ValExpr *treep2=dynamic_cast<dotat::ValExpr *>(tree2.get());

  CPPUNIT_ASSERT(treep2!=0);
  CPPUNIT_ASSERT_EQUAL(1111, treep2->val().i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), treep2->val().obj().get());
}

void TestInterp::test_expr_ss()
{
  istringstream iss1("0.d(m1).a(x).e(abcd).t(m1).ss(1).a(2)");
  dotat::RefPtr<dotat::Expr> expr1(m_interp->parse(iss1));
  dotat::Val val1=expr1->eval(*m_interp);

  CPPUNIT_ASSERT(val1.obj()->is_method("m"));
  dotat::RefPtr<dotat::Expr> tree1=val1.obj()->method("m").expr();
  dotat::VarExpr *treep1=dynamic_cast<dotat::VarExpr *>(tree1.get());

  CPPUNIT_ASSERT(treep1!=0);
  CPPUNIT_ASSERT_EQUAL(string("bc"), treep1->var_name());

  istringstream iss2("0.d(m1).a(x).e(abcd).t(m1).ss(10).a(1)");
  dotat::RefPtr<dotat::Expr> expr2(m_interp->parse(iss2));
  dotat::Val val2=expr2->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(m_interp->nil_obj().get(), val2.obj().get());
  istringstream iss3("0.d(m1).a(x).e(abcd).t(m1).ss(1).a(-1)");
  dotat::RefPtr<dotat::Expr> expr3(m_interp->parse(iss3));
  dotat::Val val3=expr3->eval(*m_interp);

  CPPUNIT_ASSERT(val3.obj()->is_method("m"));
  dotat::RefPtr<dotat::Expr> tree3=val3.obj()->method("m").expr();
  dotat::VarExpr *treep3=dynamic_cast<dotat::VarExpr *>(tree3.get());

  CPPUNIT_ASSERT(treep3!=0);
  CPPUNIT_ASSERT_EQUAL(string("bcd"), treep3->var_name());
}

void TestInterp::test_expr_sc()
{
  istringstream iss("0.d(m1).a(x).e(aaa).t(m1).sc(0.d(m).a(x).e(ddd))");
  dotat::RefPtr<dotat::Expr> expr(m_interp->parse(iss));
  dotat::Val val=expr->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(-1, val.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), val.obj().get());
}

void TestInterp::test_expr_2n()
{
 istringstream iss("0.d(m1).a(x).e(\\1234).t(m1).2n(0)");
  dotat::RefPtr<dotat::Expr> expr(m_interp->parse(iss));
  dotat::Val val=expr->eval(*m_interp);

  CPPUNIT_ASSERT_EQUAL(1234, val.i());
  CPPUNIT_ASSERT_EQUAL(m_interp->num_obj().get(), val.obj().get());
}

int main(int argc, char *argv[])
{
  CppUnit::TextUi::TestRunner runner;

  runner.addTest(TestRefPtr::suite());
  runner.addTest(TestScope::suite());
  runner.addTest(TestMethod::suite());
  runner.addTest(TestObj::suite());
  runner.addTest(TestExpr::suite());
  runner.addTest(TestParser::suite());
  runner.addTest(TestInterp::suite());
  return runner.run() ? 0 : 1;
}
