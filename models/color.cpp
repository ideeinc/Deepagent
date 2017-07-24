#include "color.h"
#include <TDebug>


Color::Color(const QString &c)
{
    ::memset(val, 0, sizeof(val[0])*4);
    QString color = c;

    if (color.startsWith("#")) {
        switch (color.length()) {
        case 4:
            color.resize(7);
            color[1] = color[2] = c[1];
            color[3] = color[4] = c[2];
            color[5] = color[6] = c[3];
            // FALL THROUGH

        case 7:
            val[0] = (uchar)color.mid(5, 2).toInt(nullptr, 16); // B
            val[1] = (uchar)color.mid(3, 2).toInt(nullptr, 16); // G
            val[2] = (uchar)color.mid(1, 2).toInt(nullptr, 16); // R
            break;

        default:
            tWarn() << "Invalid color value: " << color;
            break;
        }
    }
}
