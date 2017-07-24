#ifndef COLOR_H
#define COLOR_H

#include <QtCore>
#include <opencv2/core/core.hpp>


class Color : public CvScalar
{
public:
    Color();
    Color(int r, int g, int b, int a = 0);
    Color(const Color &other);
    explicit Color(const QString &color);
    Color &operator=(const Color &other);

    int red() const;
    int green() const;
    int blue() const;

    operator CvScalar() const { return *this; }
    operator CvScalar&() { return *this; }
    operator const CvScalar&() const { return *this; }
};


inline Color::Color()
{
    ::memset(val, 0, sizeof(val[0])*4);
}

inline Color::Color(int r, int g, int b, int a)
 {
    val[0] = b;
    val[1] = g;
    val[2] = r;
    val[3] = a;
}

inline Color::Color(const Color &other)
{
    ::memcpy(val, other.val, sizeof(val[0])*4);
}

inline Color &Color::operator=(const Color &other)
{
    ::memcpy(val, other.val, sizeof(val[0])*4);
    return *this;
}

inline int Color::red() const { return val[2]; }
inline int Color::green() const { return val[1]; }
inline int Color::blue() const { return val[0]; }

#endif // COLOR_H
