#ifndef UTILS_H
#define UTILS_H

#ifdef __unix__
inline int max (int a, int b)
{
	return (a > b) ? a : b;
}
#endif /* __unix__ */

#define neverNull(x) ((x) == NULL ? "" : (x))
#define stringToBool(x) ((x) == NULL || strncmp(neverNull(x), "true", sizeof("true") - 1) == 0)
#define boolToString(x) ((x) ? "true" : "false")


#if defined(WIN32) || defined(__WIN32) || defined(__WIN32__) || defined(WIN64) || defined(__WIN64) || defined(__WIN64__)
#define IS_WINDOWS 1
#else
#define IS_WINDOWS 0
#endif

#endif /* UTILS_H */
