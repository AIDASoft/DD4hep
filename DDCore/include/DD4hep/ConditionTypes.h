
#define DD4HEP_INSTANTIATE_GRAMMAR_TYPE(x)  namespace DD4hep { \
    template<> const BasicGrammar& BasicGrammar::instance<x>()  { static Grammar<x> s; return s;}}
