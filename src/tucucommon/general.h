#ifndef GENERAL_H
#define GENERAL_H

//template <typename T>
//void ignore(T &&)
//{ }

/// This macro can be used when a function parameter is not to be used
/// Everytime we use it we should think about a potential refactoring 
template <typename T>
void FINAL_UNUSED_PARAMETER(T &&)
{ }
// #define FINAL_UNUSED_PARAMETER(P) {ignore(P);}

/// This macro can be used when a function parameter is not yet used
/// but should be in the future.
///
/// At some stage the TMP_UNUSED_PARAMETER() should disappear
template <typename T>
void TMP_UNUSED_PARAMETER(T &&)
{ }
// #define TMP_UNUSED_PARAMETER(P) {ignore(P);}



#ifndef TUCU_NOCATCH
#define TUCU_TRY try
#define TUCU_CATCH(x) catch (x)
#define TUCU_ONEXCEPTION(x) {x}
#else
#define TUCU_TRY if (true)
#define TUCU_CATCH(x) if (false)
#define TUCU_ONEXCEPTION(x) {}
#endif


#endif // GENERAL_H
