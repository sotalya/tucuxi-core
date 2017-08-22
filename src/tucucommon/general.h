#ifndef GENERAL_H
#define GENERAL_H

template <typename T>
void ignore(T &&)
{ }

#define UNUSED_PARAMETER(P) {ignore(P);}
#define TMP_UNUSED_PARAMETER(P) {ignore(P);}

#endif // GENERAL_H
