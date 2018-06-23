template<class T>
inline const T& min(const T& a, const T& b)
{
    return (b < a) ? b : a;
}

template<class T>
inline const T& max(const T& a, const T& b)
{
    return (a < b) ? b : a;
}

//BRANCHLESS - speed? Probably slower!
template <typename T>
inline int32_t min(int32_t a, int32_t b)
{
    return b + ((a-b) & (a-b)>>31);
}

inline int32_t max(int32_t a, int32_t b)
{
    return a - ((a-b) & (a-b)>>31);
}

//BRANCHLESS - speed? Probably slower!
template <typename T>
inline T imax (T a, T b)
{
    return (a > b) * a + (a <= b) * b;
}

template <typename T>
inline T imin (T a, T b)
{
    return (a > b) * b + (a <= b) * a;
}


template<class T>
inline const T& clamp(const T& d, const T& l, const T& u)
{
    const T& t = d < l ? l : d;
    return t > u ? u : t;
}

//template<class T>
//inline const T& clamp(const T& d, const T& l, const T& u)
//{
//    return min(max(d, l), u);
//}

template<class T>
inline bool between(const T& a, const T& b, const T& c)
{
    return (a >= b) && (a <= c);
}
