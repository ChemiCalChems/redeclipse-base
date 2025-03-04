#pragma once

// script binding functionality

enum { VAL_NULL = 0, VAL_INT, VAL_FLOAT, VAL_STR, VAL_ANY, VAL_CODE, VAL_MACRO, VAL_IDENT, VAL_CSTR, VAL_CANY, VAL_WORD, VAL_POP, VAL_COND, VAL_MAX };

enum
{
    CODE_START = 0,
    CODE_OFFSET,
    CODE_NULL, CODE_TRUE, CODE_FALSE, CODE_NOT,
    CODE_POP,
    CODE_ENTER, CODE_ENTER_RESULT,
    CODE_EXIT, CODE_RESULT_ARG,
    CODE_VAL, CODE_VALI,
    CODE_DUP,
    CODE_MACRO,
    CODE_BOOL,
    CODE_BLOCK, CODE_EMPTY,
    CODE_COMPILE, CODE_COND,
    CODE_FORCE,
    CODE_RESULT,
    CODE_IDENT, CODE_IDENTU, CODE_IDENTARG,
    CODE_COM, CODE_COMD, CODE_COMC, CODE_COMV,
    CODE_CONC, CODE_CONCW, CODE_CONCM, CODE_DOWN,
    CODE_SVAR, CODE_SVARM, CODE_SVAR1,
    CODE_IVAR, CODE_IVAR1, CODE_IVAR2, CODE_IVAR3,
    CODE_FVAR, CODE_FVAR1,
    CODE_LOOKUP, CODE_LOOKUPU, CODE_LOOKUPARG,
    CODE_LOOKUPM, CODE_LOOKUPMU, CODE_LOOKUPMARG,
    CODE_ALIAS, CODE_ALIASU, CODE_ALIASARG, CODE_CALL, CODE_CALLU, CODE_CALLARG,
    CODE_PRINT,
    CODE_LOCAL,
    CODE_DO, CODE_DOARGS,
    CODE_JUMP, CODE_JUMP_TRUE, CODE_JUMP_FALSE, CODE_JUMP_RESULT_TRUE, CODE_JUMP_RESULT_FALSE,

    CODE_OP_MASK = 0x3F,
    CODE_RET = 6,
    CODE_RET_MASK = 0xC0,

    /* return type flags */
    RET_NULL   = VAL_NULL<<CODE_RET,
    RET_STR    = VAL_STR<<CODE_RET,
    RET_INT    = VAL_INT<<CODE_RET,
    RET_FLOAT  = VAL_FLOAT<<CODE_RET,
};

enum { ID_VAR = 0, ID_FVAR, ID_SVAR, ID_COMMAND, ID_ALIAS, ID_LOCAL, ID_DO, ID_DOARGS, ID_IF, ID_RESULT, ID_NOT, ID_AND, ID_OR, ID_MAX };

#define VAR_MIN INT_MIN+1
#define VAR_MAX INT_MAX-1
#define FVAR_MIN -1e6f
#define FVAR_MAX 1e6f
#define FVAR_NONZERO 1e-6f

enum
{
    IDF_INIT = 1<<0, IDF_PERSIST = 1<<1, IDF_READONLY = 1<<2, IDF_REWRITE = 1<<3, IDF_WORLD = 1<<4, IDF_COMPLETE = 1<<5,
    IDF_TEXTURE = 1<<6, IDF_CLIENT = 1<<7, IDF_SERVER = 1<<8, IDF_HEX = 1<<9, IDF_UNKNOWN = 1<<10, IDF_ARG = 1<<11,
    IDF_PRELOAD = 1<<12, IDF_GAMEPRELOAD = 1<<13, IDF_GAMEMOD = 1<<14, IDF_NAMECOMPLETE = 1<<15, IDF_EMUVAR = 1<<16
};

struct ident;
struct identval
{
    union
    {
        int i;      // ID_VAR, VAL_INT
        float f;    // ID_FVAR, VAL_FLOAT
        char *s;    // ID_SVAR, VAL_STR
        const uint *code; // VAL_CODE
        ident *id;  // VAL_IDENT
        const char *cstr; // VAL_CSTR
    };
};

struct tagval : identval
{
    int type;

    void setint(int val) { type = VAL_INT; i = val; }
    void setfloat(float val) { type = VAL_FLOAT; f = val; }
    void setnumber(double val) { i = int(val); if(val == i) type = VAL_INT; else { type = VAL_FLOAT; f = val; } }
    void setstr(char *val) { type = VAL_STR; s = val; }
    void setnull() { type = VAL_NULL; i = 0; }
    void setcode(const uint *val) { type = VAL_CODE; code = val; }
    void setmacro(const uint *val) { type = VAL_MACRO; code = val; }
    void setcstr(const char *val) { type = VAL_CSTR; cstr = val; }
    void setident(ident *val) { type = VAL_IDENT; id = val; }

    const char *getstr() const;
    int getint() const;
    float getfloat() const;
    double getnumber() const;
    bool getbool() const;
    void getval(tagval &r) const;

    void cleanup();
};

struct identstack
{
    identval val;
    int valtype;
    identstack *next;
};

union identvalptr
{
    void *p;  // ID_*VAR
    int *i;   // ID_VAR
    float *f; // ID_FVAR
    char **s; // ID_SVAR
};

typedef void (__cdecl *identfun)(ident *id);

struct ident
{
    uchar type; // one of ID_* above
    union
    {
        uchar valtype; // ID_ALIAS
        uchar numargs; // ID_COMMAND
    };
    int flags, index, level;
    const char *name;
    union
    {
        struct // ID_VAR, ID_FVAR, ID_SVAR
        {
            union
            {
                struct { int minval, maxval; };     // ID_VAR
                struct { float minvalf, maxvalf; }; // ID_FVAR
            };
            identvalptr storage;
            identval overrideval;
            identval def; // declared-default (by *init.cfg)
            identval bin; // builtin-default (hard coded or version.cfg)
        };
        struct // ID_ALIAS
        {
            uint *code;
            identval val;
            identstack *stack;
        };
        struct // ID_COMMAND
        {
            const char *args;
            uint argmask;
        };
    };
    identfun fun; // ID_VAR, ID_FVAR, ID_SVAR, ID_COMMAND
    char *desc;
    vector<char *> fields;

    ident() {}
    // ID_VAR
    ident(int t, const char *n, int m, int c, int x, int *s, void *f = NULL, int flags = IDF_COMPLETE, int level = 0)
        : type(t), flags(flags | ((flags&IDF_HEX && uint(x) == 0xFFFFFFFFU ? uint(m) > uint(x) : m > x) ? IDF_READONLY : 0)), level(level), name(n), minval(m), maxval(x), fun((identfun)f), desc(NULL)
    { fields.shrink(0); def.i = c; bin.i = c; storage.i = s; }
    // ID_FVAR
    ident(int t, const char *n, float m, float c, float x, float *s, void *f = NULL, int flags = IDF_COMPLETE, int level = 0)
        : type(t), flags(flags | ((flags&IDF_HEX && uint(x) == 0xFFFFFFFFU ? uint(m) > uint(x) : m > x) ? IDF_READONLY : 0)), level(level), name(n), minvalf(m), maxvalf(x), fun((identfun)f), desc(NULL)
    { fields.shrink(0); def.f = c; bin.f = c; storage.f = s; }
    // ID_SVAR
    ident(int t, const char *n, char *c, char **s, void *f = NULL, int flags = IDF_COMPLETE, int level = 0)
        : type(t), flags(flags), level(level), name(n), fun((identfun)f), desc(NULL)
    { fields.shrink(0); def.s = c; bin.s = newstring(c); storage.s = s; }
    // ID_ALIAS
    ident(int t, const char *n, char *a, int flags, int level)
        : type(t), valtype(VAL_STR), flags(flags), level(level), name(n), code(NULL), stack(NULL), desc(NULL)
    { fields.shrink(0); val.s = a; }
    ident(int t, const char *n, int a, int flags, int level)
        : type(t), valtype(VAL_INT), flags(flags), level(level), name(n), code(NULL), stack(NULL), desc(NULL)
    { fields.shrink(0); val.i = a; }
    ident(int t, const char *n, float a, int flags, int level)
        : type(t), valtype(VAL_FLOAT), flags(flags), level(level), name(n), code(NULL), stack(NULL), desc(NULL)
    { fields.shrink(0); val.f = a; }
    ident(int t, const char *n, int flags, int level)
        : type(t), valtype(VAL_NULL), flags(flags), level(level), name(n), code(NULL), stack(NULL), desc(NULL)
    { fields.shrink(0); }
    ident(int t, const char *n, const tagval &v, int flags, int level)
        : type(t), valtype(v.type), flags(flags), level(level), name(n), code(NULL), stack(NULL), desc(NULL)
    { fields.shrink(0); val = v; }
    // ID_COMMAND
    ident(int t, const char *n, const char *args, uint argmask, int numargs, void *f = NULL, int flags = IDF_COMPLETE, int level = 0)
        : type(t), numargs(numargs), flags(flags), level(level), name(n), args(args), argmask(argmask), fun((identfun)f), desc(NULL)
    { fields.shrink(0); }

    void changed() { if(fun) fun(this); }

    void setval(const tagval &v)
    {
        valtype = v.type;
        val = v;
    }

    void setval(const identstack &v)
    {
        valtype = v.valtype;
        val = v.val;
    }

    void forcenull()
    {
        if(valtype==VAL_STR) delete[] val.s;
        valtype = VAL_NULL;
    }

    float getfloat() const;
    int getint() const;
    double getnumber() const;
    const char *getstr() const;
    void getval(tagval &r) const;
    void getcstr(tagval &v) const;
    void getcval(tagval &v) const;
};

extern hashnameset<ident> idents;

extern void addident(ident *id);

extern tagval *commandret;
extern const char *intstr(int v);
extern const char *intstr(ident *id);
extern void intret(int v);
extern const char *floatstr(float v);
extern void floatret(float v);
extern const char *numberstr(double v);
extern void numberret(double v);
extern void stringret(char *s);
extern void result(tagval &v);
extern void result(const char *s);
extern char *conc(tagval *v, int n, bool space, const char *prefix, int prefixlen);

static inline char *conc(tagval *v, int n, bool space)
{
    return conc(v, n, space, NULL, 0);
}

static inline char *conc(tagval *v, int n, bool space, const char *prefix)
{
    return conc(v, n, space, prefix, strlen(prefix));
}

static inline int parseint(const char *s)
{
    return int(strtoul(s, NULL, 0));
}

#define PARSEFLOAT(name, type) \
    static inline type parse##name(const char *s) \
    { \
        /* not all platforms (windows) can parse hexadecimal integers via strtod */ \
        char *end; \
        double val = strtod(s, &end); \
        return val || end==s || (*end!='x' && *end!='X') ? type(val) : type(parseint(s)); \
    }
PARSEFLOAT(float, float)
PARSEFLOAT(number, double)

static inline void intformat(char *buf, int v, int len = 20) { nformatstring(buf, len, "%d", v); }
static inline void floatformat(char *buf, float v, int len = 20) { nformatstring(buf, len, v==int(v) ? "%.1f" : "%.6g", v); }
static inline void numberformat(char *buf, double v, int len = 20)
{
    int i = int(v);
    if(v == i) nformatstring(buf, len, "%d", i);
    else nformatstring(buf, len, "%.6g", v);
}

static inline const char *getstr(const identval &v, int type)
{
    switch(type)
    {
        case VAL_STR: case VAL_MACRO: case VAL_CSTR: return v.s;
        case VAL_INT: return intstr(v.i);
        case VAL_FLOAT: return floatstr(v.f);
        default: return "";
    }
}
inline const char *tagval::getstr() const { return ::getstr(*this, type); }
inline const char *ident::getstr() const { return ::getstr(val, valtype); }

#define GETNUMBER(name, ret) \
    static inline ret get##name(const identval &v, int type) \
    { \
        switch(type) \
        { \
            case VAL_FLOAT: return ret(v.f); \
            case VAL_INT: return ret(v.i); \
            case VAL_STR: case VAL_MACRO: case VAL_CSTR: return parse##name(v.s); \
            default: return ret(0); \
        } \
    } \
    inline ret tagval::get##name() const { return ::get##name(*this, type); } \
    inline ret ident::get##name() const { return ::get##name(val, valtype); }
GETNUMBER(int, int)
GETNUMBER(float, float)
GETNUMBER(number, double)

static inline void getval(const identval &v, int type, tagval &r)
{
    switch(type)
    {
        case VAL_STR: case VAL_MACRO: case VAL_CSTR: r.setstr(newstring(v.s)); break;
        case VAL_INT: r.setint(v.i); break;
        case VAL_FLOAT: r.setfloat(v.f); break;
        default: r.setnull(); break;
    }
}

inline void tagval::getval(tagval &r) const { ::getval(*this, type, r); }
inline void ident::getval(tagval &r) const { ::getval(val, valtype, r); }

inline void ident::getcstr(tagval &v) const
{
    switch(valtype)
    {
        case VAL_MACRO: v.setmacro(val.code); break;
        case VAL_STR: case VAL_CSTR: v.setcstr(val.s); break;
        case VAL_INT: v.setstr(newstring(intstr(val.i))); break;
        case VAL_FLOAT: v.setstr(newstring(floatstr(val.f))); break;
        default: v.setcstr(""); break;
    }
}

inline void ident::getcval(tagval &v) const
{
    switch(valtype)
    {
        case VAL_MACRO: v.setmacro(val.code); break;
        case VAL_STR: case VAL_CSTR: v.setcstr(val.s); break;
        case VAL_INT: v.setint(val.i); break;
        case VAL_FLOAT: v.setfloat(val.f); break;
        default: v.setnull(); break;
    }
}
extern int variable(const char *name, int min, int cur, int max, int *storage, identfun fun, int flags, int level = 0);
extern float fvariable(const char *name, float min, float cur, float max, float *storage, identfun fun, int flags, int level = 0);
extern char *svariable(const char *name, const char *cur, char **storage, identfun fun, int flags, int level = 0);
extern void setvar(const char *name, int i, bool dofunc = false, bool def = false, bool force = false);
extern void setfvar(const char *name, float f, bool dofunc = false, bool def = false, bool force = false);
extern void setsvar(const char *name, const char *str, bool dofunc = false, bool def = false);
extern void setvarchecked(ident *id, int val);
extern void setfvarchecked(ident *id, float val);
extern void setsvarchecked(ident *id, const char *val);
extern void touchvar(const char *name);
extern int getvar(const char *name);
extern int getvarmin(const char *name);
extern int getvarmax(const char *name);
extern int getvardef(const char *name, bool rb = false);
extern float getfvar(const char *name);
extern float getfvarmin(const char *name);
extern float getfvarmax(const char *name);
extern bool identexists(const char *name);
extern ident *getident(const char *name);
extern ident *newident(const char *name, int flags = 0, int level = 0);
extern ident *readident(const char *name);
extern ident *writeident(const char *name, int flags = 0, int level = 0);
extern bool addcommand(const char *name, identfun fun, const char *args, int type = ID_COMMAND, int flags = IDF_COMPLETE, int levle = 0);

extern uint *compilecode(const char *p);
extern void keepcode(uint *p);
extern void freecode(uint *p);
extern void executeret(const uint *code, tagval &result = *commandret);
extern void executeret(const char *p, tagval &result = *commandret);
extern void executeret(ident *id, tagval *args, int numargs, bool lookup = false, tagval &result = *commandret);
extern char *executestr(const uint *code);
extern char *executestr(const char *p);
extern char *executestr(ident *id, tagval *args, int numargs, bool lookup = false);
extern char *execidentstr(const char *name, bool lookup = false);
extern int execute(const uint *code);
extern int execute(const char *p);
extern int execute(const char *p, bool nonworld);
extern int execute(ident *id, tagval *args, int numargs, bool lookup = false);
extern int execident(const char *name, int noid = 0, bool lookup = false);
extern float executefloat(const uint *code);
extern float executefloat(const char *p);
extern float executefloat(ident *id, tagval *args, int numargs, bool lookup = false);
extern float execidentfloat(const char *name, float noid = 0, bool lookup = false);
extern bool executebool(const uint *code);
extern bool executebool(const char *p);
extern bool executebool(ident *id, tagval *args, int numargs, bool lookup = false);
extern bool execidentbool(const char *name, bool noid = false, bool lookup = false);
enum { EXEC_NOWORLD = 1<<0, EXEC_VERSION = 1<<1, EXEC_BUILTIN = 1<<2 };
extern bool execfile(const char *cfgfile, bool msg = true, int flags = 0);
extern void alias(const char *name, const char *action, bool world = false);
extern void alias(const char *name, tagval &v, bool world = false);
extern void worldalias(const char *name, const char *action);
extern const char *getalias(const char *name);
extern const char *escapestring(const char *s);
extern const char *escapeid(const char *s);
static inline const char *escapeid(ident &id) { return escapeid(id.name); }
extern bool validateblock(const char *s);
extern void explodelist(const char *s, vector<char *> &elems, int limit = -1);
extern char *parsetext(const char *&p);
extern char *indexlist(const char *s, int pos);
extern const char *indexlist(const char *s, int pos, int &len);
extern int listincludes(const char *list, const char *needl, int needlelen);
extern char *shrinklist(const char *list, const char *limit, int failover = 0, bool invert = false);
extern int listlen(const char *s);
extern void printreadonly(ident *id);
extern void printeditonly(ident *id);
extern void printvar(ident *id, int n, const char *str = NULL);
extern void printfvar(ident *id, float f, const char *str = NULL);
extern void printsvar(ident *id, const char *s, const char *str = NULL);
extern void printvar(ident *id);
extern int clampvar(ident *id, int i, int minval, int maxval, bool msg = true);
extern float clampfvar(ident *id, float f, float minval, float maxval, bool msg = true);
extern void loopiter(ident *id, identstack &stack, const tagval &v);
extern void loopend(ident *id, identstack &stack);

#define loopstart(id, stack) if((id)->type != ID_ALIAS) return; identstack stack;
static inline void loopiter(ident *id, identstack &stack, int i) { tagval v; v.setint(i); loopiter(id, stack, v); }
static inline void loopiter(ident *id, identstack &stack, float f) { tagval v; v.setfloat(f); loopiter(id, stack, v); }
static inline void loopiter(ident *id, identstack &stack, const char *s) { tagval v; v.setstr(newstring(s)); loopiter(id, stack, v); }

extern int identflags;

extern void checksleep(int millis);
extern void clearsleep(bool clearworlds = true);

extern char *logtimeformat, *filetimeformat;
extern int filetimelocal;
extern const char *gettime(time_t ctime = 0, const char *format = NULL);
extern bool hasflag(const char *flags, char f);
extern char *limitstring(const char *str, size_t len);

// nasty macros for registering script functions, abuses globals to avoid excessive infrastructure
#define KEYWORD(flags, name, type) UNUSED static bool __dummy_##type = addcommand(#name, (identfun)NULL, NULL, type, flags|IDF_COMPLETE)
#define COMMANDKN(flags, level, name, type, fun, nargs) UNUSED static bool __dummy_##fun = addcommand(#name, (identfun)fun, nargs, type, flags|IDF_COMPLETE, level)
#define COMMANDK(flags, name, type, nargs) COMMANDKN(flags, 0, name, type, name, nargs)
#define COMMANDN(flags, name, fun, nargs) COMMANDKN(flags, 0, name, ID_COMMAND, fun, nargs)
#define COMMAND(flags, name, nargs) COMMANDN(flags, name, name, nargs)

// anonymous inline commands, uses nasty template trick with line numbers to keep names unique
#define ICOMMANDNAME(name) _icmd_##name
#define ICOMMANDSNAME _icmds_
#define ICOMMANDKNS(flags, level, name, type, cmdname, nargs, proto, b) template<int N> struct cmdname; template<> struct cmdname<__LINE__> { static bool init; static void run proto; }; bool cmdname<__LINE__>::init = addcommand(name, (identfun)cmdname<__LINE__>::run, nargs, type, flags|IDF_COMPLETE, level); void cmdname<__LINE__>::run proto \
    { b; }
#define ICOMMANDKN(flags, level, name, type, cmdname, nargs, proto, b) ICOMMANDKNS(flags, level, #name, type, cmdname, nargs, proto, b)
#define ICOMMANDK(flags, name, type, nargs, proto, b) ICOMMANDKN(flags, 0, name, type, ICOMMANDNAME(name), nargs, proto, b)
#define ICOMMANDKS(flags, name, type, nargs, proto, b) ICOMMANDKNS(flags, 0, name, type, ICOMMANDSNAME, nargs, proto, b)
#define ICOMMANDNS(flags, name, cmdname, nargs, proto, b) ICOMMANDKNS(flags, 0, name, ID_COMMAND, cmdname, nargs, proto, b)
#define ICOMMANDN(flags, name, cmdname, nargs, proto, b) ICOMMANDNS(flags, #name, cmdname, nargs, proto, b)
#define ICOMMAND(flags, name, nargs, proto, b) ICOMMANDN(flags, name, ICOMMANDNAME(name), nargs, proto, b)
#define ICOMMANDS(flags, name, nargs, proto, b) ICOMMANDNS(flags, name, ICOMMANDSNAME, nargs, proto, b)
#define ICOMMANDV(flags, name, b) ICOMMANDN(flags, name, ICOMMANDNAME(name), "N$", (int *numargs, ident *id), \
{ \
    if(*numargs > 0) printreadonly(id); \
    else if(*numargs < 0) intret((b)); \
    else printvar(id, (b)); \
})
#define ICOMMANDVF(flags, name, b) ICOMMANDN(flags, name, ICOMMANDNAME(name), "N$", (int *numargs, ident *id), \
{ \
    if(*numargs > 0) printreadonly(id); \
    else if(*numargs < 0) floatret((b)); \
    else printfvar(id, (b)); \
})
#define ICOMMANDVS(flags, name, b) ICOMMANDN(flags, name, ICOMMANDNAME(name), "N$", (int *numargs, ident *id), \
{ \
    if(*numargs > 0) printreadonly(id); \
    else if(*numargs < 0) result((b)); \
    else printsvar(id, (b)); \
})

#define _VAR(name, global, min, cur, max, flags, level) int global = variable(#name, min, cur, max, &global, NULL, flags|IDF_COMPLETE, level)
#define VARN(flags, name, global, min, cur, max) _VAR(name, global, min, cur, max, flags, 0)
#define VAR(flags, name, min, cur, max) _VAR(name, name, min, cur, max, flags, 0)
#define _VARF(name, global, min, cur, max, body, flags, level)  void var_##name(ident *id); int global = variable(#name, min, cur, max, &global, var_##name, flags|IDF_COMPLETE, level); void var_##name(ident *id) { body; }
#define VARFN(flags, name, global, min, cur, max, body) _VARF(name, global, min, cur, max, body, flags, 0)
#define VARF(flags, name, min, cur, max, body) _VARF(name, name, min, cur, max, body, flags, 0)

#define _FVAR(name, global, min, cur, max, flags, level) float global = fvariable(#name, min, cur, max, &global, NULL, flags|IDF_COMPLETE, level)
#define FVARN(flags, name, global, min, cur, max) _FVAR(name, global, min, cur, max, flags, 0)
#define FVAR(flags, name, min, cur, max) _FVAR(name, name, min, cur, max, flags, 0)
#define _FVARF(name, global, min, cur, max, body, flags, level) void var_##name(ident *id); float global = fvariable(#name, min, cur, max, &global, var_##name, flags|IDF_COMPLETE, level); void var_##name(ident *id) { body; }
#define FVARFN(flags, name, global, min, cur, max, body) _FVARF(name, global, min, cur, max, body, flags, 0)
#define FVARF(flags, name, min, cur, max, body) _FVARF(name, name, min, cur, max, body, flags, 0)

#define _SVAR(name, global, cur, flags, level) char *global = svariable(#name, cur, &global, NULL, flags|IDF_COMPLETE, level)
#define SVARN(flags, name, global, cur) _SVAR(name, global, cur, flags, 0)
#define SVAR(flags, name, cur) _SVAR(name, name, cur, flags, 0)
#define _SVARF(name, global, cur, body, flags, level) void var_##name(ident *id); char *global = svariable(#name, cur, &global, var_##name, flags|IDF_COMPLETE, level); void var_##name(ident *id) { body; }
#define SVARFN(flags, name, global, cur, body) _SVARF(name, global, cur, body, flags, 0)
#define SVARF(flags, name, cur, body) _SVARF(name, name, cur, body, flags, 0)

#define _CVAR(name, cur, init, body, flags, level) bvec name = bvec::fromcolor(cur); _VARF(name, _##name, 0, cur, 0xFFFFFF, { init; name = bvec::fromcolor(_##name); body; }, IDF_HEX|flags, level)
#define CVARF(flags, name, cur, body) _CVAR(name, cur, , body, flags, 0)
#define CVAR(flags, name, cur) _CVAR(name, cur, , , flags, 0)
#define CVAR0F(flags, name, cur, body) _CVAR(name, cur, { if(!_##name) _##name = cur; }, body, flags, 0)
#define CVAR1F(flags, name, cur, body) _CVAR(name, cur, { if(_##name <= 255) _##name |= (_##name<<8) | (_##name<<16); }, body, flags, 0)
#define CVAR0(flags, name, cur) CVAR0F(flags, name, cur, )
#define CVAR1(flags, name, cur) CVAR1F(flags, name, cur, )

// game world controlling stuff
#define WITHWORLD(body) { int _oldflags = identflags; identflags |= IDF_WORLD; body; identflags = _oldflags; }
#define RUNWORLD(n) { ident *wid = idents.access(n); if(wid && wid->type==ID_ALIAS && wid->flags&IDF_WORLD) { WITHWORLD(execute(wid->getstr())); } }

#if defined(CPP_GAME_MAIN)
#define IDF_GAME (IDF_CLIENT|IDF_REWRITE)
#define G(name) (name)
#define PHYS(name) (G(name)*G(name##scale))
#define GICOMMAND(flags, level, name, nargs, proto, svbody, ccbody) ICOMMANDKNS(flags|IDF_GAME, level, #name, ID_COMMAND, ICOMMANDNAME(name), nargs, proto, ccbody)
#define GVARN(flags, level, name, global, min, cur, max) _VAR(name, global, min, cur, max, flags|IDF_GAME, level)
#define GVAR(flags, level, name, min, cur, max) _VAR(name, name, min, cur, max, flags|IDF_GAME, level)
#define GVARF(flags, level, name, min, cur, max, svbody, ccbody) _VARF(name, name, min, cur, max, ccbody, flags|IDF_GAME, level)
#define GFVARN(flags, level, name, global, min, cur, max) _FVAR(name, global, min, cur, max, flags|IDF_GAME, level)
#define GFVAR(flags, level, name, min, cur, max) _FVAR(name, name, min, cur, max, flags|IDF_GAME, level)
#define GFVARF(flags, level, name, min, cur, max, svbody, ccbody) _FVARF(name, name, min, cur, max, ccbody, flags|IDF_GAME, level)
#define GSVARN(flags, level, name, global, cur) _SVAR(name, global, cur, flags|IDF_GAME, level)
#define GSVAR(flags, level, name, cur) _SVAR(name, name, cur, flags|IDF_GAME, level)
#define GSVARF(flags, level, name, cur, svbody, ccbody) _SVARF(name, name, cur, ccbody, flags|IDF_GAME, level)
#elif defined(CPP_GAME_SERVER)
#define G(name) (sv_##name)
#define IDF_GAME (IDF_SERVER|IDF_REWRITE)
#define GICOMMAND(flags, level, name, nargs, proto, svbody, ccbody) ICOMMANDKNS(flags|(IDF_GAME&~IDF_REWRITE), level, "sv_" #name, ID_COMMAND, ICOMMANDNAME(sv_##name), nargs, proto, svbody)
#define GVARN(flags, level, name, global, min, cur, max) _VAR(sv_##name, global, min, cur, max, flags|IDF_GAME, level)
#define GVAR(flags, level, name, min, cur, max) _VAR(sv_##name, sv_##name, min, cur, max, flags|IDF_GAME, level)
#define GVARF(flags, level, name, min, cur, max, svbody, ccbody) _VARF(sv_##name, sv_##name, min, cur, max, svbody, flags|IDF_GAME, level)
#define GFVARN(flags, level, name, global, min, cur, max) _FVAR(sv_##name, global, min, cur, max, flags|IDF_GAME, level)
#define GFVAR(flags, level, name, min, cur, max) _FVAR(sv_##name, sv_##name, min, cur, max, flags|IDF_GAME, level)
#define GFVARF(flags, level, name, min, cur, max, svbody, ccbody) _FVARF(sv_##name, sv_##name, min, cur, max, svbody, flags|IDF_GAME, level)
#define GSVARN(flags, level, name, global, cur) _SVAR(sv_##name, global, cur, flags|IDF_GAME, level)
#define GSVAR(flags, level, name, cur) _SVAR(sv_##name, sv_##name, cur, flags|IDF_GAME, level)
#define GSVARF(flags, level, name, cur, svbody, ccbody) _SVARF(sv_##name, sv_##name, cur, svbody, flags|IDF_GAME, level)
#else
#define G(name) (name)
#define PHYS(name) (G(name)*G(name##scale))
#define GICOMMAND(flags, level, n, g, proto, svbody, ccbody)
#define GVARN(flags, level, name, global, min, cur, max) extern int name
#define GVAR(flags, level, name, min, cur, max) extern int name
#define GVARF(flags, level, name, min, cur, max, svbody, ccbody) extern int name
#define GVARN(flags, level, name, global, min, cur, max) extern int name
#define GVAR(flags, level, name, min, cur, max) extern int name
#define GVARF(flags, level, name, min, cur, max, svbody, ccbody) extern int name
#define GFVARN(flags, level, name, global, min, cur, max) extern float name
#define GFVAR(flags, level, name, min, cur, max) extern float name
#define GFVARF(flags, level, name, min, cur, max, svbody, ccbody) extern float name
#define GSVARN(flags, level, name, global, cur) extern char *name
#define GSVAR(flags, level, name, cur) extern char *name
#define GSVARF(flags, level, name, cur, svbody, ccbody) extern char *name
#endif

#ifdef CPP_ENGINE_COMMAND
SVAR(IDF_READONLY, validxname, "null int float str any code macro ident cstr cany word pop cond");
VAR(IDF_READONLY, validxnull, 0, VAL_NULL, -1);
SVAR(IDF_READONLY, valnamenull, "Null");
VAR(IDF_READONLY, validxint, 0, VAL_INT, -1);
SVAR(IDF_READONLY, valnameint, "Integer");
VAR(IDF_READONLY, validxfloat, 0, VAL_FLOAT, -1);
SVAR(IDF_READONLY, valnamefloat, "Float");
VAR(IDF_READONLY, validxstr, 0, VAL_STR, -1);
SVAR(IDF_READONLY, valnamestr, "String");
VAR(IDF_READONLY, validxany, 0, VAL_ANY, -1);
SVAR(IDF_READONLY, valnameany, "Any");
VAR(IDF_READONLY, validxcode, 0, VAL_CODE, -1);
SVAR(IDF_READONLY, valnamecode, "Code");
VAR(IDF_READONLY, validxmacro, 0, VAL_MACRO, -1);
SVAR(IDF_READONLY, valnamemacro, "Macro");
VAR(IDF_READONLY, validxident, 0, VAL_IDENT, -1);
SVAR(IDF_READONLY, valnameident, "Identifier");
VAR(IDF_READONLY, validxcstr, 0, VAL_CSTR, -1);
SVAR(IDF_READONLY, valnamecstr, "Constant-string");
VAR(IDF_READONLY, validxcany, 0, VAL_CANY, -1);
SVAR(IDF_READONLY, valnamecany, "Constant-any");
VAR(IDF_READONLY, validxword, 0, VAL_WORD, -1);
SVAR(IDF_READONLY, valnameworld, "Word");
VAR(IDF_READONLY, validxpop, 0, VAL_POP, -1);
SVAR(IDF_READONLY, valnamepop, "Pop");
VAR(IDF_READONLY, validxcond, 0, VAL_COND, -1);
SVAR(IDF_READONLY, valnamecond, "Conditional");
VAR(IDF_READONLY, validxmax, 0, VAL_MAX, -1);
SVAR(IDF_READONLY, ididxname, "var fvar svar command alias local do doargs if result not and or");
VAR(IDF_READONLY, ididxvar, 0, ID_VAR, -1);
SVAR(IDF_READONLY, idnamevar, "Integer-variable");
VAR(IDF_READONLY, ididxfvar, 0, ID_FVAR, -1);
SVAR(IDF_READONLY, idnamefvar, "Float-variable");
VAR(IDF_READONLY, ididxsvar, 0, ID_SVAR, -1);
SVAR(IDF_READONLY, idnamesvar, "String-variable");
VAR(IDF_READONLY, ididxcommand, 0, ID_COMMAND, -1);
SVAR(IDF_READONLY, idnamecommand, "Command");
VAR(IDF_READONLY, ididxalias, 0, ID_ALIAS, -1);
SVAR(IDF_READONLY, idnamealias, "Alias");
VAR(IDF_READONLY, ididxlocal, 0, ID_LOCAL, -1);
SVAR(IDF_READONLY, idnamelocal, "Local");
VAR(IDF_READONLY, ididxdo, 0, ID_DO, -1);
SVAR(IDF_READONLY, idnamedo, "Do");
VAR(IDF_READONLY, ididxdoargs, 0, ID_DOARGS, -1);
SVAR(IDF_READONLY, idnamedoargs, "Do-arguments");
VAR(IDF_READONLY, ididxif, 0, ID_IF, -1);
SVAR(IDF_READONLY, idnameif, "If-condition");
VAR(IDF_READONLY, ididxresult, 0, ID_RESULT, -1);
SVAR(IDF_READONLY, idnameresult, "Result");
VAR(IDF_READONLY, ididxnot, 0, ID_NOT, -1);
SVAR(IDF_READONLY, idnamenot, "Not-condition");
VAR(IDF_READONLY, ididxand, 0, ID_AND, -1);
SVAR(IDF_READONLY, idnameand, "And-condition");
VAR(IDF_READONLY, ididxor, 0, ID_OR, -1);
SVAR(IDF_READONLY, idnameor, "Or-condition");
VAR(IDF_READONLY, ididxmax, 0, ID_MAX, -1);
SVAR(IDF_READONLY, idfidxname, "init persist readonly rewrite world complete texture client server hex unknown arg preload gamepreload gamemod namecomplete");
VAR(IDF_READONLY, idfbitinit, 0, IDF_INIT, -1);
SVAR(IDF_READONLY, idfnameinit, "Initialiser");
VAR(IDF_READONLY, idfbitpersist, 0, IDF_PERSIST, -1);
SVAR(IDF_READONLY, idfnamepersist, "Persistent");
VAR(IDF_READONLY, idfbitreadonly, 0, IDF_READONLY, -1);
SVAR(IDF_READONLY, idfnamereadonly, "Read-only");
VAR(IDF_READONLY, idfbitrewrite, 0, IDF_REWRITE, -1);
SVAR(IDF_READONLY, idfnamerewrite, "Rewrite");
VAR(IDF_READONLY, idfbitworld, 0, IDF_WORLD, -1);
SVAR(IDF_READONLY, idfnameworld, "World");
VAR(IDF_READONLY, idfbitcomplete, 0, IDF_COMPLETE, -1);
SVAR(IDF_READONLY, idfnamecomplete, "Complete");
VAR(IDF_READONLY, idfbittexture, 0, IDF_TEXTURE, -1);
SVAR(IDF_READONLY, idfnametexture, "Texture");
VAR(IDF_READONLY, idfbitclient, 0, IDF_CLIENT, -1);
SVAR(IDF_READONLY, idfnameclient, "Client");
VAR(IDF_READONLY, idfbitserver, 0, IDF_SERVER, -1);
SVAR(IDF_READONLY, idfnameserver, "Server");
VAR(IDF_READONLY, idfbithex, 0, IDF_HEX, -1);
SVAR(IDF_READONLY, idfnamehex, "Hexadecimal");
VAR(IDF_READONLY, idfbitunknown, 0, IDF_UNKNOWN, -1);
SVAR(IDF_READONLY, idfnameunknown, "Unknown");
VAR(IDF_READONLY, idfbitarg, 0, IDF_ARG, -1);
SVAR(IDF_READONLY, idfnamearg, "Argument");
VAR(IDF_READONLY, idfbitpreload, 0, IDF_PRELOAD, -1);
SVAR(IDF_READONLY, idfnamepreload, "Preload");
VAR(IDF_READONLY, idfbitgamepreload, 0, IDF_GAMEPRELOAD, -1);
SVAR(IDF_READONLY, idfnamegamepreload, "Game-preload");
VAR(IDF_READONLY, idfbitgamemod, 0, IDF_GAMEMOD, -1);
SVAR(IDF_READONLY, idfnamegamemod, "Game-modifier");
VAR(IDF_READONLY, idfbitnamecomplete, 0, IDF_NAMECOMPLETE, -1);
SVAR(IDF_READONLY, idfnamenamecomplete, "Name-complete");
VAR(IDF_READONLY, varidxmax, 0, VAR_MAX, -1);
VAR(IDF_READONLY, varidxmin, 0, VAR_MIN, -1);
FVAR(IDF_READONLY, fvaridxmax, 0, FVAR_MAX, -1);
FVAR(IDF_READONLY, fvaridxmin, 0, FVAR_MIN, -1);
FVAR(IDF_READONLY, fvaridxnonzero, 0, FVAR_NONZERO, -1);
#endif // CPP_ENGINE_COMMAND
