#include "jpeginfo.h"


JpegInfo::JpegInfo(const QString &file)
{
    parse(file);
}

void JpegInfo::parse(const QString &file)
{
    QFile jpeg(file);
    if (! jpeg.open(QIODevice::ReadOnly)) {
        return;
    }

    QDataStream ds(&jpeg);
    ds.setByteOrder(QDataStream::BigEndian);

    quint16 soi, marker, len;
    ds >> soi;
    if (soi != 0xFFD8) {
        return;
    }

    while (! ds.atEnd()) {
        ds >> marker >> len;

        switch (marker) {
        case 0xFFE0: { // APP0
            ds.skipRawData(len - 2);
            break; }

        case 0xFFC0: { // SOF0 Baseline
            ds >> _bit >> _height >> _width >> _comps;
            goto parse_end;
            break; }

        case 0xFFC2: { // SOF2 Progressive
            ds >> _bit >> _height >> _width >> _comps;
            goto parse_end;
            break; }

        default:
            ds.skipRawData(len - 2);
            break;
        }
    }

parse_end:
    jpeg.close();
}
