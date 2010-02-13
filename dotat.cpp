//
// dotat.cpp - .@ Langauge
// by £ukasz Szpakowski
//

#include <cctype>
#include <sstream>
#include "dotat.hpp"

using namespace std;

namespace dotat
{
  //
  // GCObj
  //

  GCObj::~GCObj()
  {
  }

  //
  // Expr
  //

  Expr::~Expr()
  {
  }

  Val Expr::eval_in(Interp &interp, const Ptr<Tail> &tail)
  {
    RefPtr<Scope> tmp_top_scope=interp.top_scope();
    Val tmp_r;

    {
      PopScopeGuard ps_guard(interp, tmp_top_scope);

      //interp.pop_scope();
      tmp_r=eval(interp, tail);
      //interp.push_scope(tmp_top_scope);
    }
    return tmp_r;
  }

  const RefPtr<Expr> Expr::left() const
  {
    return RefPtr<Expr>();
  }

  const RefPtr<Expr> Expr::right() const
  {
    return RefPtr<Expr>();
  }

  bool Expr::set_left(const RefPtr<Expr> &/*left*/)
  {
    return false;
  }

  bool Expr::set_right(const RefPtr<Expr> &/*right*/)
  {
    return false;
  }

  RefPtr<Expr> Expr::substit(const string &/*var_name*/, const RefPtr<Expr> &/*expr*/)
  {
    return RefPtr<Expr>(this);
  }

  //
  // Val
  //

  Val Val::send_method(const string &name, Interp &interp, const RefPtr<Expr> &arg, const Ptr<Tail> &tail) const
  {
    if(m_obj->is_method(name)) {
      Method tmp_method=m_obj->method(name);
      Val tmp_r=tmp_method.call(interp, *this, arg, tail);

      interp.set_last_method(tmp_method);
      return tmp_r;
    } else {
      interp.set_last_method(Method());
      return interp.nil_val();
    }
  }

  //
  // Var
  //

  Val Var::eval(Interp &interp, const Ptr<Tail> &tail) const
  {
    PushScopeGuard ps_guard(interp, m_scope);
    Val tmp_r;

    //interp.push_scope(m_scope);
    tmp_r=m_expr->eval(interp, tail);
    //interp.pop_scope();
    return tmp_r;
  }

  //
  // Scope
  //

  Scope::~Scope()
  {
  }

  Val Scope::eval_var(const string &name, Interp &interp, const Ptr<Tail> &tail) const
  {
    if(is_var(name)) {
      Var tmp_var=var(name);

      return tmp_var.eval(interp, tail);
    } else {
      return interp.nil_val();
    }
  }

  //
  // Method
  //

  Val Method::call(Interp &interp, const Val &rcvr, const RefPtr<Expr> &arg, const Ptr<Tail> &tail) const
  {
    PushScopeGuard ps_guard(interp, new Scope(rcvr.obj()->scope()));
    Val tmp_r;

    //interp.push_scope(new Scope(rcvr.obj()->scope()));
    if(!m_has_first_self) {
      interp.top_scope()->set_self(rcvr);
    } else {
      interp.top_scope()->set_self(interp.top_scope()->first_self());
      interp.top_scope()->set_first_self(Val());
    }
    if(m_expr.get()!=0) {
      interp.top_scope()->def_var(m_arg_name, Var(arg, interp.prev_scope()));
#if 0
      tmp_r=m_expr->eval(interp);
#else
      if(tail.get()==0) {
        Tail tmp_tail;

        while((tmp_r=m_expr->eval(interp, &tmp_tail)).obj().get()==0) {
          interp.pop_scope();
          interp.push_scope(tmp_tail.scope);
        }
      } else {
        tmp_r=m_expr->eval(interp, tail);
      }
#endif
    } else {
      tmp_r=(*m_fptr)(interp, arg, m_data);
    }
    //interp.pop_scope();
    return tmp_r;
  }

  //
  // Obj
  //

  Obj::~Obj()
  {
  }

  Ptr<Obj> Obj::clone() const
  {
    return Ptr<Obj>(new Obj(*this));
  }

  //
  // ValExpr
  //

  ValExpr::~ValExpr()
  {
  }

  Ptr<Expr> ValExpr::clone() const
  {
    return Ptr<Expr>(new ValExpr(*this));
  }

  Val ValExpr::eval(Interp &interp, const Ptr<Tail> &tail)
  {
#if 1
    // detect tail recursion
    if(tail.get()!=0 && tail->expr.get()==this) {
      tail->scope=interp.top_scope();
      return Val();
    }
#endif
    return m_val;
  }

  string ValExpr::str() const
  {
    ostringstream oss;

    oss << m_val.i();
    return oss.str();
  }

  //
  // VarExpr
  //

  VarExpr::~VarExpr()
  {
  }

  Ptr<Expr> VarExpr::clone() const
  {
    return Ptr<Expr>(new VarExpr(*this));
  }

  Val VarExpr::eval(Interp &interp, const Ptr<Tail> &tail)
  {
#if 1
    // detect tail recursion
    if(tail.get()!=0 && tail->expr.get()==this) {
      tail->scope=interp.top_scope();
      return Val();
    }
#endif
    return interp.top_scope()->eval_var(m_var_name, interp, tail);
  }

  string VarExpr::str() const
  {
    return m_var_name;
  }

  RefPtr<Expr> VarExpr::substit(const string &var_name, const RefPtr<Expr> &expr)
  {
    return m_var_name==var_name ? expr : RefPtr<Expr>(this);
  }

  //
  // SelfExpr
  //

  SelfExpr::~SelfExpr()
  {
  }

  Ptr<Expr> SelfExpr::clone() const
  {
    return Ptr<Expr>(new SelfExpr(*this));
  }

  Val SelfExpr::eval(Interp &interp, const Ptr<Tail> &tail)
  {
#if 1
    // detect tail recursion
    if(tail.get()!=0 && tail->expr.get()==this) {
      tail->scope=interp.top_scope();
      return Val();
    }
#endif
    return interp.top_scope()->self();
  }

  string SelfExpr::str() const
  {
    return "@";
  }

  //
  // SendMethodExpr
  //

  SendMethodExpr::~SendMethodExpr()
  {
  }

  Ptr<Expr> SendMethodExpr::clone() const
  {
    return Ptr<Expr>(new SendMethodExpr(*this));
  }

  Val SendMethodExpr::eval(Interp &interp, const Ptr<Tail> &tail)
  {
#if 1
    if(tail.get()!=0) {
      // detect tail recursion
      if(tail->expr.get()==this) {
        tail->scope=interp.top_scope();
        return Val();
      }
      if(tail->expr.get()==0) {
        tail->expr=Ptr<Expr>(this);
      }
    }
#endif
    if(!is_eval()) {
      Val tmp_r, tmp_rcvr=rcvr()->eval(interp);

      tmp_r=tmp_rcvr.send_method(m_method_name, interp, m_arg, tail);
#if 1
      if(tmp_r.obj().get()!=0) {
        // evaluated value
        const Method &last_method=interp.last_method();

        set_determ(m_rcvr->is_determ() && last_method.is_determ());
        if(m_rcvr->is_eval()) {
          if(last_method.is_determ() && (!last_method.can_eval_arg() || !m_arg->has_var())) {
            set_eval(true);
            m_eval_val=tmp_r;
          }
        }
      }
#endif
      return tmp_r;
    } else {
      return m_eval_val;
    }
  }

  string SendMethodExpr::str() const
  {
    return m_rcvr->str()+m_method_name+m_arg->str();
  }

  const RefPtr<Expr> SendMethodExpr::left() const
  {
    return m_rcvr;
  }

  const RefPtr<Expr> SendMethodExpr::right() const
  {
    return m_arg;
  }

  bool SendMethodExpr::set_left(const RefPtr<Expr> &left)
  {
    m_rcvr=left;
    set_var_after();
    return true;
  }

  bool SendMethodExpr::set_right(const RefPtr<Expr> &right)
  {
    m_arg=right;
    set_var_after();
    return true;
  }

  RefPtr<Expr> SendMethodExpr::substit(const string &var_name, const RefPtr<Expr> &expr)
  {
    RefPtr<Expr> new_rcvr=m_rcvr->substit(var_name, expr);
    RefPtr<Expr> new_arg=m_arg->substit(var_name, expr);

    if(m_rcvr.get()!=new_rcvr.get() || m_arg.get()!=new_arg.get()) {
      RefPtr<Expr> tmp_r(this->clone());

      tmp_r->set_left(new_rcvr);
      tmp_r->set_right(new_arg);
      return tmp_r;
    } else {
      return RefPtr<Expr>(this);
    }
  }

  //
  // ParseError
  //

  ParseError::ParseError(unsigned n_line, const std::string &str)
  {
    ostringstream oss;

    oss << n_line;
    m_msg=oss.str()+": "+str;
  }

  ParseError::~ParseError() throw()
  {
  }

  const char * ParseError::what() const throw()
  {
    return m_msg.c_str();
  }

  //
  // Parser
  //

  RefPtr<Expr> Parser::parse(const Interp &interp, int delim)
  {
    RefPtr<Expr> expr;
    char c;

    skip_spaces();
    expr=parse_rcvr(interp, delim);
    skip_spaces();
    while(get_c(c)) {
      if(c==delim) {
        unget_c();
        break;
      }
      if(c!='.') {
        throw ParseError(m_n_line, "Expected \'.\'");
      }
      skip_spaces();
      string tmp_method_name=parse_method_name();

      skip_spaces();
      if(!get_c(c) || c!='(') {
        throw ParseError(m_n_line, "Expected \'(\'");
      }
      RefPtr<Expr> tmp_arg=parse(interp, ')');

      if(!get_c(c) || c!=')') {
        throw ParseError(m_n_line, "Expected \')\'");
      }
      skip_spaces();
      expr=Ptr<Expr>(new SendMethodExpr(expr, tmp_method_name, tmp_arg));
    }
    return expr;
  }

  bool Parser::get_c(char &c)
  {
    if(m_is.get(c).good()) {
      if(c=='\n') {
        m_n_line++;
      }
    }
    return m_is.good();
  }

  bool Parser::unget_c()
  {
    m_is.unget();
    if(m_is.peek()=='\n') {
      m_n_line--;
    }
    return m_is.good();
  }

  void Parser::skip_comment()
  {
    char c, tmp_c;

    while(get_c(c)) {
      if(c=='@') {
        if(get_c(tmp_c)) {
          if(tmp_c=='.') {
            return ;
          }
          unget_c();
        }
      }
    }
    throw ParseError(m_n_line, "Unterminated comment");
  }

  void Parser::skip_spaces()
  {
    char c, tmp_c;

    while(get_c(c)) {
      if(c=='.') {
        if(get_c(tmp_c)) {
          if(tmp_c=='@') {
            skip_comment();
            continue;
          }
          unget_c();
        }
      }
      if(!isspace(c)) {
        unget_c();
        break;
      }
    }
  }

  RefPtr<Expr> Parser::parse_rcvr(const Interp &interp, int delim)
  {
    string tmp_str;
    char c;
    bool must_var=false;

    while(get_c(c)) {
      if(isspace(c) || c=='.' || c==delim) {
        unget_c();
        break;
      }
      if(c=='\\') {
        if(!get_c(c)) {
          throw ParseError(m_n_line, "Unexpected EOF");
        }
        must_var=true;
      }
      tmp_str+=c;
    }
    if(!must_var) {
      unsigned i=0;
      bool is_num=true;

      if(i<tmp_str.length() && tmp_str[i]=='-') {
        i++;
      }
      if(i+1<=tmp_str.length()) {
        for(; i<tmp_str.length(); i++) {
          if(!isdigit(tmp_str[i])) {
            is_num=false;
            break;
          }
        }
      } else {
        is_num=false;
      }
      if(is_num) {
        istringstream iss(tmp_str);
        int n;

        iss >> n;
        return new ValExpr(interp.num_val(n));
      } else if(tmp_str=="@") {
        return new SelfExpr;
      } else {
        return new VarExpr(tmp_str);
      }
    } else {
      return new VarExpr(tmp_str);
    }
  }

  string Parser::parse_method_name()
  {
    string new_method_name;
    char c, tmp_c;

    while(get_c(c)) {
      if(c=='.') {
        if(get_c(tmp_c)) {
          if(tmp_c=='@') {
            unget_c();
            unget_c();
            break;
          }
          unget_c();
        }
      }
      if(isspace(c)!=0 || c=='(') {
        unget_c();
        break;
      }
      if(c=='\\') {
        if(!get_c(c)) {
          throw ParseError(m_n_line, "Unexpected EOF");
        }
      }
      new_method_name+=c;
    }
    return new_method_name;
  }

  //
  // Interp
  //

  Interp::Interp(unsigned scope_stck_size)
  {
    m_scope_stck.reserve(scope_stck_size);
    m_scope_stck.push_back(new Scope);
    init_any_obj();
    init_nil_obj();
    init_nonil_obj();
    init_num_obj();
    init_method_obj();
    init_arg_obj();
    init_expr_obj();
    m_in=Ptr<istream>(&cin);
    m_out=Ptr<ostream>(&cout);
  }

  Interp::~Interp()
  {
  }

  namespace
  {
    //
    // any
    //

    Val any_d(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      Val tmp_self=interp.top_scope()->self();
      Val new_method(0, interp.method_obj()->clone());

      new_method.obj()->set_rcvr(tmp_self);
      new_method.obj()->set_method_name(arg->str());
      return new_method;
    }

    Val any_u(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      Val tmp_self=interp.top_scope()->self();
      Val new_self(tmp_self.i(), tmp_self.obj()->clone());

      new_self.obj()->undef_method(arg->str());
      return new_self;
    }

    Val any_t(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      Val tmp_self=interp.top_scope()->self();
      string method_name=arg->str();

      if(tmp_self.obj()->is_method(method_name)) {
        const Method &method=tmp_self.obj()->method(method_name);
        const string &arg_name=method.arg_name();
        RefPtr<Expr> tmp_tree=method.expr();
        Val new_expr(0, interp.expr_obj()->clone());

        new_expr.obj()->def_method("m", Method(arg_name, tmp_tree));
        return new_expr;
      } else {
        return interp.nil_val();
      }
    }

    Val any_g(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      char c;

      if(interp.in()->get(c).good()) {
        Val new_expr(0, interp.expr_obj()->clone());
        string new_str(1, c);
        RefPtr<Expr> new_tree(new VarExpr(new_str));

        new_expr.obj()->def_method("m", Method(new_str, new_tree));
        return new_expr;
      } else {
        return interp.nil_val();
      }
    }

    Val any_p(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      *(interp.out()) << arg->str();
      return interp.num_val(0);
    }

    //
    // nil
    //

    Val nil_not(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      return interp.num_val(0);
    }

    Val nil_and(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      return interp.nil_val();
    }

#if 0
    Val nil_or(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      return arg->eval_in(interp);
    }
#endif

    //
    // nonil
    //

    Val nonil_not(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      return interp.nil_val();
    }

#if 0
    Val nonil_and(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      return arg->eval_in(interp);
    }
#endif

    Val nonil_or(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      return interp.top_scope()->self();
    }

    //
    // num
    //

    Val num_add(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      Val tmp_self=interp.top_scope()->self();
      Val tmp_arg=arg->eval_in(interp);

      return interp.num_val(tmp_self.i()+tmp_arg.i());
    }

    Val num_sub(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      Val tmp_self=interp.top_scope()->self();
      Val tmp_arg=arg->eval_in(interp);

      return interp.num_val(tmp_self.i()-tmp_arg.i());
    }

    Val num_mul(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      Val tmp_self=interp.top_scope()->self();
      Val tmp_arg=arg->eval_in(interp);

      return interp.num_val(tmp_self.i()*tmp_arg.i());
    }

    Val num_div(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      Val tmp_self=interp.top_scope()->self();
      Val tmp_arg=arg->eval_in(interp);

      if(tmp_arg.i()!=0) {
        return interp.num_val(tmp_self.i()/tmp_arg.i());
      } else {
        return interp.nil_val();
      }
    }

    Val num_mod(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      Val tmp_self=interp.top_scope()->self();
      Val tmp_arg=arg->eval_in(interp);

      if(tmp_arg.i()!=0) {
        return interp.num_val(tmp_self.i()%tmp_arg.i());
      } else {
        return interp.nil_val();
      }
    }

    Val num_equ(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      Val tmp_self=interp.top_scope()->self();
      Val tmp_arg=arg->eval_in(interp);

      if(tmp_self.i()==tmp_arg.i()) {
        return interp.num_val(0);
      } else {
        return interp.nil_val();
      }
    }

    Val num_neq(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      Val tmp_self=interp.top_scope()->self();
      Val tmp_arg=arg->eval_in(interp);

      if(tmp_self.i()!=tmp_arg.i()) {
        return interp.num_val(0);
      } else {
        return interp.nil_val();
      }
    }

    Val num_lt(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      Val tmp_self=interp.top_scope()->self();
      Val tmp_arg=arg->eval_in(interp);

      if(tmp_self.i()<tmp_arg.i()) {
        return interp.num_val(0);
      } else {
        return interp.nil_val();
      }
    }

    Val num_le(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      Val tmp_self=interp.top_scope()->self();
      Val tmp_arg=arg->eval_in(interp);

      if(tmp_self.i()<=tmp_arg.i()) {
        return interp.num_val(0);
      } else {
        return interp.nil_val();
      }
    }

    Val num_gt(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      Val tmp_self=interp.top_scope()->self();
      Val tmp_arg=arg->eval_in(interp);

      if(tmp_self.i()>tmp_arg.i()) {
        return interp.num_val(0);
      } else {
        return interp.nil_val();
      }
    }

    Val num_ge(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      Val tmp_self=interp.top_scope()->self();
      Val tmp_arg=arg->eval_in(interp);

      if(tmp_self.i()>=tmp_arg.i()) {
        return interp.num_val(0);
      } else {
        return interp.nil_val();
      }
    }

    Val num_2t(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      Val tmp_self=interp.top_scope()->self();
      ostringstream oss;

      oss << tmp_self.i();
      RefPtr<Expr> new_tree(new VarExpr(oss.str()));
      Val new_expr(0, interp.expr_obj()->clone());

      new_expr.obj()->def_method("m", Method(oss.str(), new_tree));
      return new_expr;
    }

    //
    // method
    //

    Val method_a(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      Val tmp_self=interp.top_scope()->self();

      if(tmp_self.obj()->is_arg_name()) {
        Val new_method(0, interp.method_obj()->clone());

        new_method.obj()->set_rcvr(tmp_self);
        new_method.obj()->set_method_name("a");
        new_method.obj()->set_arg_name(arg->str());
        return new_method;
      } else {
        Val new_self(tmp_self.i(), tmp_self.obj()->clone());

        new_self.obj()->set_arg_name(arg->str());
        return new_self;
      }
    }

    Val arg_a(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &data);

    Val method_e(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      Val tmp_prev=interp.top_scope()->self();
      Val tmp_rcvr=tmp_prev.obj()->rcvr();
      Method new_method;
      RefPtr<Obj> new_data;

      if(tmp_rcvr.obj()->rcvr().obj().get()!=0) {
        new_method=Method(tmp_prev.obj()->arg_name(), arg, true);
        do {
          new_data=interp.arg_obj()->clone();
          new_data->def_method("a", new_method);
          new_data->set_arg_name(tmp_rcvr.obj()->arg_name());
          new_method=Method(arg_a, new_data);
          tmp_prev=tmp_rcvr;
          tmp_rcvr=tmp_prev.obj()->rcvr();
        } while(tmp_rcvr.obj()->rcvr().obj().get()!=0);
      } else {
        new_method=Method(tmp_prev.obj()->arg_name(), arg);
      }
      Val new_rcvr(tmp_rcvr.i(), tmp_rcvr.obj()->clone());

      new_rcvr.obj()->def_method(tmp_prev.obj()->method_name(), new_method);
      return new_rcvr;
    }

    //
    // arg
    //

    Val arg_a(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &data)
    {
      RefPtr<Obj> new_data(data->clone());
      Var new_var(arg, interp.prev_scope());

      new_data->set_scope(*(interp.top_scope()));
      new_data->scope().def_var(data->arg_name(), new_var);
      if(new_data->scope().first_self().obj().get()==0) {
        new_data->scope().set_first_self(interp.top_scope()->self());
      }
      return Val(0, new_data);
    }

    //
    // expr
    //

    Val expr_lr(Interp &interp, bool is_left)
    {
      Val tmp_self=interp.top_scope()->self();

      if(tmp_self.obj()->is_method("m")) {
        const Method &method=tmp_self.obj()->method("m");
        const string &arg_name=method.arg_name();
        RefPtr<Expr> tmp_tree=method.expr();
        RefPtr<Expr> tmp_lr=(is_left ? tmp_tree->left() : tmp_tree->right());

        if(tmp_tree.get()!=0 && tmp_lr.get()!=0) {
          Val new_self(tmp_self.i(), tmp_self.obj()->clone());

          new_self.obj()->def_method("m", Method(arg_name, tmp_lr));
          return new_self;
        }
      }
      return interp.nil_val();
    }

    Val expr_l(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      return expr_lr(interp, true);
    }

    Val expr_r(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      return expr_lr(interp, false);
    }

    Val expr_slr(Interp &interp, const RefPtr<Expr> &arg, bool is_left)
    {
      Val tmp_self=interp.top_scope()->self();
      Val tmp_arg=arg->eval_in(interp);

      if(tmp_self.obj()->is_method("m") && tmp_arg.obj()->is_method("m")) {
        const Method &method1=tmp_self.obj()->method("m");
        const string &arg_name1=method1.arg_name();
        const Method &method2=tmp_arg.obj()->method("m");
        RefPtr<Expr> tmp_tree=method1.expr();
        RefPtr<Expr> tmp_lr=method2.expr();

        if(tmp_tree.get()!=0 && tmp_lr.get()!=0) {
          RefPtr<Expr> new_tree(tmp_tree->clone());
          bool tmp_r;

          if(is_left) {
            tmp_r=new_tree->set_left(tmp_lr);
          } else {
            tmp_r=new_tree->set_right(tmp_lr);
          }
          if(tmp_r) {
            Val new_self(tmp_self.i(), tmp_self.obj()->clone());

            new_self.obj()->def_method("m", Method(arg_name1, new_tree));
            return new_self;
          }
        }
      }
      return interp.nil_val();
    }

    Val expr_sl(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      return expr_slr(interp, arg, true);
    }

    Val expr_sr(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      return expr_slr(interp, arg, false);
    }

    Val expr_s(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      Val tmp_self=interp.top_scope()->self();
      Val tmp_arg=arg->eval_in(interp);

      if(tmp_self.obj()->is_method("m") && tmp_arg.obj()->is_method("m")) {
        string var_name=interp.top_scope()->var("v").expr()->str();
        const Method &method1=tmp_self.obj()->method("m");
        const string &arg_name1=method1.arg_name();
        const Method &method2=tmp_arg.obj()->method("m");
        RefPtr<Expr> tmp_tree=method1.expr();
        RefPtr<Expr> tmp_expr=method2.expr();

        if(tmp_tree.get()!=0 && tmp_expr.get()!=0) {
          RefPtr<Expr> new_tree=tmp_tree->substit(var_name, tmp_expr);
          Val new_self(tmp_self.i(), tmp_self.obj()->clone());

          new_self.obj()->def_method("m", Method(arg_name1, new_tree));
          return new_self;
        }
      }
      return interp.nil_val();
    }

    Val expr_ss(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      Val tmp_self=interp.top_scope()->self();
      Val tmp_arg1=interp.top_scope()->var("i").expr()->eval_in(interp);
      Val tmp_arg2=arg->eval_in(interp);

      if(tmp_self.obj()->is_method("m")) {
        const Method &method=tmp_self.obj()->method("m");
        const string &arg_name=method.arg_name();
        RefPtr<Expr> tmp_tree=method.expr();

        if(tmp_tree.get()!=0) {
          string tmp_str=tmp_tree->str();
          string::size_type i=tmp_arg1.i();
          string::size_type n=(tmp_arg2.i()!=-1 ? tmp_arg2.i() : string::npos);

          if(i+n>tmp_str.length()) {
            n=string::npos;
          }
          if(i<=tmp_str.length()) {
            RefPtr<Expr> new_tree(new VarExpr(tmp_str.substr(i, n)));
            Val new_self(tmp_self.i(), tmp_self.obj()->clone());

            new_self.obj()->def_method("m", Method(arg_name, new_tree));
            return new_self;
          }
        }
      }
      return interp.nil_val();
    }

    Val expr_sc(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      Val tmp_self=interp.top_scope()->self();
      Val tmp_arg=arg->eval_in(interp);

      if(tmp_self.obj()->is_method("m") && tmp_arg.obj()->is_method("m")) {
        const Method &method1=tmp_self.obj()->method("m");
        const Method &method2=tmp_arg.obj()->method("m");
        RefPtr<Expr> tmp_tree1=method1.expr();
        RefPtr<Expr> tmp_tree2=method2.expr();

        if(tmp_tree1.get()!=0 && tmp_tree2.get()!=0) {
          string tmp_str1=tmp_tree1->str();
          string tmp_str2=tmp_tree2->str();

          return interp.num_val(tmp_str1.compare(tmp_str2));
        }
      }
      return interp.nil_val();
    }

    Val expr_2n(Interp &interp, const RefPtr<Expr> &arg, const RefPtr<Obj> &/*data*/)
    {
      Val tmp_self=interp.top_scope()->self();

      if(tmp_self.obj()->is_method("m")) {
        const Method &method=tmp_self.obj()->method("m");
        RefPtr<Expr> tmp_tree=method.expr();

        if(tmp_tree.get()!=0) {
          istringstream iss(tmp_tree->str());
          int n;

          iss >> n;
          if(iss.eof()) {
            return interp.num_val(n);
          }
        }
      }
      return interp.nil_val();
    }
  }

  void Interp::init_any_obj()
  {
    m_any_obj=RefPtr<Obj>(new Obj);
    m_any_obj->def_method("d", Method(any_d, true));
    m_any_obj->def_method("u", Method(any_u, true));
    m_any_obj->def_method("t", Method(any_t, true));
    m_any_obj->def_method("g", Method(any_g, false, true));
    m_any_obj->def_method("p", Method(any_p));
  }

  void Interp::init_nil_obj()
  {
    m_nil_obj=RefPtr<Obj>(m_any_obj->clone());
    m_nil_obj->def_method("~", Method(nil_not, true));
    m_nil_obj->def_method("&", Method(nil_and, true, false));
    //m_nil_obj->def_method("|", Method(nil_or, true));
    m_nil_obj->def_method("|", Method("x", new VarExpr("x")));
  }

  void Interp::init_nonil_obj()
  {
    m_nonil_obj=RefPtr<Obj>(m_any_obj->clone());
    m_nonil_obj->def_method("~", Method(nonil_not, true, false));
    //m_nonil_obj->def_method("&", Method(nonil_and, true));
    m_nonil_obj->def_method("&", Method("x", new VarExpr("x")));
    m_nonil_obj->def_method("|", Method(nonil_or, true, false));
  }

  void Interp::init_num_obj()
  {
    m_num_obj=RefPtr<Obj>(m_nonil_obj->clone());
    m_num_obj->def_method("+", Method(num_add, true));
    m_num_obj->def_method("-", Method(num_sub, true));
    m_num_obj->def_method("*", Method(num_mul, true));
    m_num_obj->def_method("/", Method(num_div, true));
    m_num_obj->def_method("%", Method(num_mod, true));
    m_num_obj->def_method("=", Method(num_equ, true));
    m_num_obj->def_method("<>", Method(num_neq, true));
    m_num_obj->def_method("<", Method(num_lt, true));
    m_num_obj->def_method("<=", Method(num_le, true));
    m_num_obj->def_method(">", Method(num_gt, true));
    m_num_obj->def_method(">=", Method(num_ge, true));
    m_num_obj->def_method("2t", Method(num_2t, true, false));
  }

  void Interp::init_method_obj()
  {
    m_method_obj=RefPtr<Obj>(m_nonil_obj->clone());
    m_method_obj->def_method("a", Method(method_a, true));
    m_method_obj->def_method("e", Method(method_e, true));
  }

  void Interp::init_arg_obj()
  {
    m_arg_obj=RefPtr<Obj>(m_nonil_obj->clone());
    //m_arg_obj->def_method("a", Method(arg_a));
  }

  void Interp::init_expr_obj()
  {
    m_expr_obj=RefPtr<Obj>(m_nonil_obj->clone());
    m_expr_obj->def_method("l", Method(expr_l, true, false));
    m_expr_obj->def_method("r", Method(expr_r, true, false));
    m_expr_obj->def_method("sl", Method(expr_sl, true));
    m_expr_obj->def_method("sr", Method(expr_sr, true));
    // s
    {
      RefPtr<Obj> s_data(arg_obj()->clone());

      s_data->def_method("a", Method(expr_s, true, true, true));
      s_data->set_arg_name("v");
      m_expr_obj->def_method("s", Method(arg_a, s_data));
    }
    // ss
    {
      RefPtr<Obj> ss_data(arg_obj()->clone());

      ss_data->def_method("a", Method(expr_ss, true, true, true));
      ss_data->set_arg_name("i");
      m_expr_obj->def_method("ss", Method(arg_a, ss_data));
    }
    m_expr_obj->def_method("sc", Method(expr_sc, true));
    m_expr_obj->def_method("2n", Method(expr_2n, true, false));
  }
}
