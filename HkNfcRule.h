#ifndef HK_NFCRULE_H
#define HK_NFCRULE_H

#define HKNFCRW_USE_NFCA
#define HKNFCRW_USE_NFCB
#define HKNFCRW_USE_NFCF
//#define HKNFCRW_USE_NFCDEP
//#define HKNFCRW_USE_LLCPI
//#define HKNFCRW_USE_LLCPT
#define HKNFCRW_USE_SNEPI
//#define HKNFCRW_USE_SNEPT

#if defined(HKNFCRW_USE_SNEPI) && !defined(HKNFCRW_USE_LLCPI)
#define HKNFCRW_USE_LLCPI

#ifndef HKNFCRW_USE_NFCDEP
#define HKNFCRW_USE_NFCDEP
#endif

#endif

#if defined(HKNFCRW_USE_SNEPT) && !defined(HKNFCRW_USE_LLCPT)
#define HKNFCRW_USE_LLCPT

#ifndef HKNFCRW_USE_NFCDEP
#define HKNFCRW_USE_NFCDEP
#endif

#endif


#endif /* HK_NFCRULE_H */
