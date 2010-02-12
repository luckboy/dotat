//
// dotat.hpp - .@ Langauge
// by £ukasz Szpakowski
//

#ifndef _DOTAT_HPP
#define _DOTAT_HPP

#include <exception>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <utility>

namespace dotat
{
  template<class T>
  class Ptr;

  template<class T>
  class RefPtr;

  //
  // Ptr<T>
  //

  template<class T>
  class Ptr
  {
    T *m_ptr;
  public:
    Ptr(const RefPtr<T> &rp);
    template<class U>
    Ptr(const RefPtr<U> &rp);
    Ptr & operator=(const RefPtr<T> &rp);
    template<class U>
    Ptr & operator=(const RefPtr<U> &rp);

    Ptr()
      : m_ptr(0)
    {
    }

    Ptr(T *ptr)
      : m_ptr(ptr)
    {
    }

    template<class U>
    Ptr(U *ptr)
      : m_ptr(static_cast<T *>(ptr))
    {
    }

    Ptr(const Ptr &p)
      : m_ptr(p.get())
    {
    }

    template<class U>
    Ptr(const Ptr<U> &p)
      : m_ptr(static_cast<T *>(p.get()))
    {
    }

    Ptr & operator=(const Ptr &rp)
    {
      m_ptr=rp.get();
      return *this;
    }

    template<class U>
    Ptr & operator=(const Ptr<U> &rp)
    {
      m_ptr=static_cast<T *>(rp.get());
      return *this;
    }

    T & operator*() const
    {
      return *m_ptr;
    }

    T * operator->() const
    {
      return m_ptr;
    }

    T * get() const
    {
      return m_ptr;
    }
  };

  //
  // RefPtr<T>
  //

  template<class T>
  class RefPtr
  {
    T *m_ptr;

    void reg_ptr() const
    {
      if(m_ptr!=0) {
        m_ptr->gc_inc_count();
      }
    }

    void unreg_ptr() const
    {
      if(m_ptr!=0) {
        m_ptr->gc_dec_count();
        if(m_ptr->gc_count_equ(0)) {
          delete m_ptr;
        }
      }
    }
  public:
    RefPtr(const Ptr<T> &p);
    template<class U>
    RefPtr(const Ptr<U> &p);
    RefPtr & operator=(const Ptr<T> &p);
    template<class U>
    RefPtr & operator=(const Ptr<U> &p);

    RefPtr()
      : m_ptr(0)
    {
    }

    RefPtr(T *ptr)
      : m_ptr(ptr)
    {
      reg_ptr();
    }

    template<class U>
    RefPtr(U *ptr)
      : m_ptr(static_cast<T *>(ptr))
    {
      reg_ptr();
    }

    RefPtr(const RefPtr &rp)
      : m_ptr(rp.get())
    {
      reg_ptr();
    }

    template<class U>
    RefPtr(const RefPtr<U> &rp)
      : m_ptr(static_cast<T *>(rp.get()))
    {
      reg_ptr();
    }

    ~RefPtr()
    {
      unreg_ptr();
    }

    RefPtr & operator=(const RefPtr &rp)
    {
      if(m_ptr!=rp.get()) {
        unreg_ptr();
        m_ptr=rp.get();
        reg_ptr();
      }
      return *this;
    }

    template<class U>
    RefPtr & operator=(const RefPtr<U> &rp)
    {
      if(m_ptr!=static_cast<T *>(rp.get())) {
        unreg_ptr();
        m_ptr=static_cast<T *>(rp.get());
        reg_ptr();
      }
      return *this;
    }

    T & operator*() const
    {
      return *m_ptr;
    }

    T * operator->() const
    {
      return m_ptr;
    }

    T * get() const
    {
      return m_ptr;
    }
  };

  template<class T>
  inline Ptr<T>::Ptr(const RefPtr<T> &rp)
    : m_ptr(rp.get())
  {
  }

  template<class T>
  template<class U>
  inline Ptr<T>::Ptr(const RefPtr<U> &rp)
    : m_ptr(static_cast<T *>(rp.get()))
  {
  }

  template<class T>
  inline Ptr<T> & Ptr<T>::operator=(const RefPtr<T> &rp)
  {
    m_ptr=rp.get();
    return *this;
  }

  template<class T>
  template<class U>
  inline Ptr<T> & Ptr<T>::operator=(const RefPtr<U> &rp)
  {
    m_ptr=static_cast<T *>(rp.get());
    return *this;
  }

  template<class T>
  inline RefPtr<T>::RefPtr(const Ptr<T> &p)
    : m_ptr(p.get())
  {
    reg_ptr();
  }

  template<class T>
  template<class U>
  inline RefPtr<T>::RefPtr(const Ptr<U> &p)
    : m_ptr(static_cast<T *>(p.get()))
  {
    reg_ptr();
  }

  template<class T>
  inline RefPtr<T> & RefPtr<T>::operator=(const Ptr<T> &p)
  {
    if(m_ptr!=p.get()) {
      unreg_ptr();
      m_ptr=p.get();
      reg_ptr();
    }
    return *this;
  }

  template<class T>
  template<class U>
  inline RefPtr<T> & RefPtr<T>::operator=(const Ptr<U> &p)
  {
    if(m_ptr!=static_cast<T *>(p.get())) {
      unreg_ptr();
      m_ptr=static_cast<T *>(p.get());
      reg_ptr();
    }
    return *this;
  }

  //
  // GCObj
  //

  class GCObj
  {
    int m_count;
  public:
    virtual ~GCObj();

    GCObj()
      : m_count(0)
    {
    }

    GCObj(const GCObj &)
      : m_count(0)
    {
    }

    GCObj & operator=(const GCObj &)
    {
      m_count=0;
      return *this;
    }

    bool gc_count_equ(int n) const
    {
      return m_count==n;
    }

    void gc_inc_count()
    {
      m_count++;
    }

    void gc_dec_count()
    {
      m_count--;
    }
  };

  class Expr;
  class Val;
  class Scope;
  class Method;
  class Obj;
  class ValExpr;
  class VarExpr;
  class SendMethodExpr;
  class Parser;
  class ParseError;
  class Interp;

  //
  // Expr
  //

  class Expr
    : public GCObj
  {
    bool m_is_eval;
    bool m_is_determ;
    bool m_has_var;
  public:
    virtual ~Expr();
    virtual Ptr<Expr> clone() const=0;
    virtual Val eval(Interp &/*interp*/)=0;
    virtual std::string str() const=0;
    Val eval_in(Interp &interp);
    virtual const RefPtr<Expr> left() const;
    virtual const RefPtr<Expr> right() const;
    virtual bool set_left(const RefPtr<Expr> &/*left*/);
    virtual bool set_right(const RefPtr<Expr> &/*right*/);
    virtual RefPtr<Expr> substit(const std::string &/*var_name*/, const RefPtr<Expr> &/*expr*/);
  protected:
    Expr(bool b1=false, bool b2=false, bool b3=false)
      : m_is_eval(b1), m_is_determ(b2), m_has_var(b3)
    {
    }
  public:
    bool is_eval() const
    {
      return m_is_eval;
    }

    bool is_determ() const
    {
      return m_is_determ;
    }

    bool has_var() const
    {
      return m_has_var;
    }
  protected:
    void set_eval(bool b)
    {
      m_is_eval=b;
    }

    void set_determ(bool b)
    {
      m_is_determ=b;
    }

    void set_var(bool b)
    {
      m_has_var=b;
    }
  };

  //
  // Val
  //

  class Val
  {
    int m_i;
    RefPtr<Obj> m_obj;
  public:
    Val send_method(const std::string &name, Interp &interp, const RefPtr<Expr> &arg) const;

    Val()
      : m_i(0)
    {
    }

    Val(int i, const RefPtr<Obj> &obj)
      : m_i(i), m_obj(obj)
    {
    }

    int i() const
    {
      return m_i;
    }

    const RefPtr<Obj> & obj() const
    {
      return m_obj;
    }
  };

  //
  // Var
  //

  class Var
  {
    RefPtr<Expr> m_expr;
    RefPtr<Scope> m_scope;
  public:
    Val eval(Interp &interp) const;

    Var()
    {
    }

    Var(const RefPtr<Expr> &expr, const RefPtr<Scope> scope)
      : m_expr(expr), m_scope(scope)
    {
    }

    const RefPtr<Expr> & expr() const
    {
      return m_expr;
    }

    const RefPtr<Scope> & scope() const
    {
      return m_scope;
    }
  };

  //
  // Scope
  //

  class Scope
    : public GCObj
  {
    std::map<std::string, Var> m_vars;
    Val m_self;
    Val m_first_self;
  public:
    virtual ~Scope();
    Val eval_var(const std::string &name, Interp &interp) const;

    bool is_var(const std::string &name) const
    {
      return m_vars.find(name)!=m_vars.end();
    }

    const Var & var(const std::string &name) const
    {
      return m_vars.find(name)->second;
    }

    void def_var(const std::string &name, const Var &var)
    {
      m_vars.erase(name);
      m_vars.insert(std::make_pair(name, var));
    }

    void undef_var(const std::string &name)
    {
      m_vars.erase(name);
    }

    const Val & self() const
    {
      return m_self;
    }

    void set_self(const Val &self)
    {
      m_self=self;
    }

    const Val & first_self() const
    {
      return m_first_self;
    }

    void set_first_self(const Val &self)
    {
      m_first_self=self;
    }
  };

  //
  // Method
  //

  class Method
  {
    std::string m_arg_name;
    RefPtr<Expr> m_expr;
    Val (*m_fptr)(Interp &, const RefPtr<Expr> &, const RefPtr<Obj> &);
    RefPtr<Obj> m_data;
    bool m_is_determ;
    bool m_can_eval_arg;
    bool m_has_first_self;
  public:
    Val call(Interp &interp, const Val &rcvr, const RefPtr<Expr> &arg) const;

    Method()
      : m_fptr(0), m_is_determ(true), m_can_eval_arg(true), m_has_first_self(false)
    {
    }

    Method(const std::string &arg_name, const RefPtr<Expr> &expr, bool has_first_self=false)
      : m_arg_name(arg_name), m_expr(expr), m_fptr(0), m_is_determ(false), m_can_eval_arg(true), m_has_first_self(has_first_self)
    {
    }

    Method(Val (*fptr)(Interp &, const RefPtr<Expr> &, const RefPtr<Obj> &), bool is_determ=false, bool can_eval_arg=true, bool has_first_self=false)
      : m_fptr(fptr), m_is_determ(is_determ), m_can_eval_arg(can_eval_arg), m_has_first_self(has_first_self)
    {
    }

    Method(Val (*fptr)(Interp &, const RefPtr<Expr> &, const RefPtr<Obj> &), const RefPtr<Obj> &data, bool is_determ=false, bool can_eval_arg=true, bool has_first_self=false)
      : m_fptr(fptr), m_data(data), m_is_determ(is_determ), m_can_eval_arg(can_eval_arg), m_has_first_self(has_first_self)
    {
    }

    const std::string & arg_name() const
    {
      return m_arg_name;
    }

    const RefPtr<Expr> & expr() const
    {
      return m_expr;
    }

    const RefPtr<Obj> & data() const
    {
      return m_data;
    }

    bool is_determ() const
    {
      if(m_expr.get()!=0) {
        return m_expr->is_determ();
      } else {
        return m_is_determ;
      }
    }

    bool can_eval_arg() const
    {
      return m_can_eval_arg;
    }

    bool has_first_self() const
    {
      return m_has_first_self;
    }
  };

  //
  // Obj
  //

  class Obj
    : public GCObj
  {
    std::map<std::string, Method> m_methods;
    Scope m_scope;
    Val m_rcvr;
    std::string m_method_name;
    std::string m_arg_name;
    bool m_is_arg_name;
  public:
    virtual ~Obj();
    virtual Ptr<Obj> clone() const;

    Obj()
      : m_is_arg_name(false)
    {
    }

    Obj(const Val &rcvr, const std::string &method_name)
      : m_rcvr(rcvr), m_method_name(method_name), m_is_arg_name(false)
    {
    }

    bool is_method(const std::string &name) const
    {
      return m_methods.find(name)!=m_methods.end();
    }

    const Method & method(const std::string &name) const
    {
      return m_methods.find(name)->second;
    }

    void def_method(const std::string &name, const Method &method)
    {
      m_methods.erase(name);
      m_methods.insert(std::make_pair(name, method));
    }

    void undef_method(const std::string &name)
    {
      m_methods.erase(name);
    }

    const Scope & scope() const
    {
      return m_scope;
    }

    Scope & scope()
    {
      return m_scope;
    }

    void set_scope(const Scope &scope)
    {
      m_scope=scope;
    }

    const Val & rcvr() const
    {
      return m_rcvr;
    }

    void set_rcvr(const Val &rcvr)
    {
      m_rcvr=rcvr;
    }

    const std::string & method_name() const
    {
      return m_method_name;
    }

    void set_method_name(const std::string &method_name)
    {
      m_method_name=method_name;
    }

    bool is_arg_name() const
    {
      return m_is_arg_name;
    }

    const std::string & arg_name() const
    {
      return m_arg_name;
    }

    void set_arg_name(const std::string &arg_name)
    {
      m_arg_name=arg_name;
      m_is_arg_name=true;
    }

    void unset_arg_name()
    {
      m_arg_name="";
      m_is_arg_name=true;
    }
  };

  //
  // ValExpr
  //

  class ValExpr
    : public Expr
  {
    Val m_val;
  public:
    virtual ~ValExpr();
    virtual Ptr<Expr> clone() const;
    virtual Val eval(Interp &interp);
    virtual std::string str() const;

    ValExpr(const Val &val)
      : Expr(true, true, false), m_val(val)
    {
    }

    const Val & val() const
    {
      return m_val;
    }
  };

  //
  // VarExpr
  //

  class VarExpr
    : public Expr
  {
    std::string m_var_name;
  public:
    virtual ~VarExpr();
    virtual Ptr<Expr> clone() const;
    virtual Val eval(Interp &interp);
    virtual std::string str() const;
    virtual RefPtr<Expr> substit(const std::string &var_name, const RefPtr<Expr> &expr);

    VarExpr(const std::string &var_name)
      : Expr(false, true, true), m_var_name(var_name)
    {
    }

    const std::string & var_name() const
    {
      return m_var_name;
    }
  };

  //
  // SelfExpr
  //

  class SelfExpr
    : public Expr
  {
  public:
    virtual ~SelfExpr();
    virtual Ptr<Expr> clone() const;
    virtual Val eval(Interp &interp);
    virtual std::string str() const;

    SelfExpr()
      : Expr(false, true, true)
    {
    }
  };

  //
  // SendMethodExpr
  //

  class SendMethodExpr
    : public Expr
  {
    RefPtr<Expr> m_rcvr;
    std::string m_method_name;
    RefPtr<Expr> m_arg;
    Val m_eval_val;
  public:
    virtual ~SendMethodExpr();
    virtual Ptr<Expr> clone() const;
    virtual Val eval(Interp &interp);
    virtual std::string str() const;
    virtual const RefPtr<Expr> left() const;
    virtual const RefPtr<Expr> right() const;
    virtual bool set_left(const RefPtr<Expr> &left);
    virtual bool set_right(const RefPtr<Expr> &right);
    virtual RefPtr<Expr> substit(const std::string &var_name, const RefPtr<Expr> &expr);

    SendMethodExpr(const RefPtr<Expr> &rcvr, const std::string &method_name, const RefPtr<Expr> &arg)
      : Expr(false), m_rcvr(rcvr), m_method_name(method_name), m_arg(arg)
    {
      set_var_after();
    }

    SendMethodExpr(const SendMethodExpr &expr)
      : Expr(false), m_rcvr(expr.m_rcvr), m_method_name(expr.m_method_name), m_arg(expr.m_arg)
    {
      set_var_after();
    }

    const RefPtr<Expr> & rcvr() const
    {
      return m_rcvr;
    }

    const std::string & method_name() const
    {
      return m_method_name;
    }

    const RefPtr<Expr> & arg() const
    {
      return m_arg;
    }
  private:
    void set_var_after()
    {
      set_var(m_rcvr->has_var() || m_arg->has_var());
    }
  };

  //
  // ParseError
  //

  class ParseError
    : public std::exception
  {
    std::string m_msg;
  public:
    ParseError(unsigned n_line, const std::string &str);
    virtual ~ParseError() throw();
    virtual const char* what() const throw();
  };

  //
  // Parser
  //

  class Parser
  {
    std::istream &m_is;
    unsigned m_n_line;
  public:
    RefPtr<Expr> parse(const Interp &interp, int delim=256);
  private:
    bool get_c(char &c);
    bool unget_c();
    void skip_comment();
    void skip_spaces();
    RefPtr<Expr> parse_rcvr(const Interp &interp, int delim);
    std::string parse_method_name();
  public:
    Parser(std::istream &is)
      : m_is(is), m_n_line(1)
    {
    }
  };

  //
  // Interp
  //

  class Interp
  {
    std::vector<RefPtr<Scope> > m_scope_stck;
    Method m_last_method;
    RefPtr<Obj> m_any_obj;
    RefPtr<Obj> m_nil_obj;
    RefPtr<Obj> m_nonil_obj;
    RefPtr<Obj> m_num_obj;
    RefPtr<Obj> m_method_obj;
    RefPtr<Obj> m_arg_obj;
    RefPtr<Obj> m_expr_obj;
    Ptr<std::istream> m_in;
    Ptr<std::ostream> m_out;
  public:
    Interp(unsigned scope_stck_size=1000000);
    virtual ~Interp();
  private:
    void init_any_obj();
    void init_nil_obj();
    void init_nonil_obj();
    void init_num_obj();
    void init_method_obj();
    void init_arg_obj();
    void init_expr_obj();
  public:
    unsigned scopes_count() const
    {
      return m_scope_stck.size();
    }

    void push_scope()
    {
      m_scope_stck.push_back(new Scope);
    }

    void push_scope(const RefPtr<Scope> &scope)
    {
      m_scope_stck.push_back(scope);
    }

    void pop_scope()
    {
      m_scope_stck.pop_back();
    }

    const RefPtr<Scope> & top_scope() const
    {
      return m_scope_stck.back();
    }

    const RefPtr<Scope> & prev_scope() const
    {
      return m_scope_stck[m_scope_stck.size()-2];
    }

    const Method & last_method() const
    {
      return m_last_method;
    }

    void set_last_method(const Method &method)
    {
      m_last_method=method;
    }

    Val num_val(int n) const
    {
      return Val(n, m_num_obj);
    }

    Val nil_val() const
    {
      return Val(0, m_nil_obj);
    }

    const RefPtr<Obj> & nil_obj() const
    {
      return m_nil_obj;
    }

    const RefPtr<Obj> & num_obj() const
    {
      return m_num_obj;
    }

    const RefPtr<Obj> & method_obj() const
    {
      return m_method_obj;
    }

    const RefPtr<Obj> & arg_obj() const
    {
      return m_arg_obj;
    }

    const RefPtr<Obj> & expr_obj() const
    {
      return m_expr_obj;
    }
  protected:
    void set_nil_obj(const RefPtr<Obj> &obj)
    {
      m_nil_obj=obj;
    }


    void set_num_obj(const RefPtr<Obj> &obj)
    {
      m_num_obj=obj;
    }

    void set_method_obj(const RefPtr<Obj> &obj)
    {
      m_method_obj=obj;
    }

    void set_arg_obj(const RefPtr<Obj> &obj)
    {
      m_arg_obj=obj;
    }

    void set_expr_obj(const RefPtr<Obj> &obj)
    {
      m_expr_obj=obj;
    }
  public:
    RefPtr<Expr> parse(std::istream &is) const
    {
      Parser parser(is);

      return parser.parse(*this);
    }

    void interp(const RefPtr<Expr> &expr)
    {
      expr->eval(*this);
    }

    void interp(std::istream &is)
    {
      RefPtr<Expr> expr=parse(is);

      expr->eval(*this);
    }

    const Ptr<std::istream> & in() const
    {
      return m_in;
    }

    void set_in(const Ptr<std::istream> &in)
    {
      m_in=in;
    }

    const Ptr<std::ostream> & out() const
    {
      return m_out;
    }

    void set_out(const Ptr<std::ostream> &out)
    {
      m_out=out;
    }
  };

  class PushScopeGuard
  {
    Interp &m_interp;

    PushScopeGuard(PushScopeGuard &);
    PushScopeGuard & operator=(PushScopeGuard &);
  public:
    PushScopeGuard(Interp &interp)
      : m_interp(interp)
    {
      m_interp.push_scope();
    }

    PushScopeGuard(Interp &interp, const RefPtr<Scope> &scope)
      : m_interp(interp)
    {
      m_interp.push_scope(scope);
    }

    ~PushScopeGuard()
    {
      m_interp.pop_scope();
    }
  };

  class PopScopeGuard
  {
    Interp &m_interp;
    const RefPtr<Scope> &m_scope;

    PopScopeGuard(PopScopeGuard &);
    PopScopeGuard & operator=(PopScopeGuard &);
  public:
    PopScopeGuard(Interp &interp, const RefPtr<Scope> &scope)
      : m_interp(interp), m_scope(scope)
    {
      m_interp.pop_scope();
    }

    ~PopScopeGuard()
    {
      m_interp.push_scope(m_scope);
    }
  };
}

#endif
