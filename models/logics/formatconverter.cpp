#include <QtCore>
#include <QDomDocument>
#include <TDebug>
#include "formatconverter.h"
#include "image.h"

#ifdef STANDALONE_USE
# undef tError
# undef tWarn
# undef tDebug
# define tError qCritical
# define tWarn qWarning
# define tDebug qDebug
#endif


static void createDomTextElement(const QString &name, const QString &text, QDomElement &parent, QDomDocument &document)
{
    auto tag = document.createElement(name);
    parent.appendChild(tag);
    tag.appendChild(document.createTextNode(text));
}


static void createDomElement(const QVariantMap &map, QDomElement &parent, QDomDocument &document)
{
    for (QMapIterator<QString, QVariant> it(map); it.hasNext(); ) {
        it.next();
        auto tag = document.createElement(it.key());
        parent.appendChild(tag);

        auto text = document.createTextNode(it.value().toString());
        tag.appendChild(text);
    }
}


FormatConverter::FormatConverter(const QString &srcFilePath, Type type, const QString &imageFilePath)
    : _srcType(type), _imageFile(imageFilePath)
{
    parse(srcFilePath);
}


QString FormatConverter::convertTo(Type type) const
{
    QString outData;

    switch (type) {
    case Type::KITTI:
        break;

    case Type::PASCAL_VOC: {
        QDomDocument doc;
        auto root = doc.createElement("annotation");
        doc.appendChild(root);

        QString filename = QFileInfo(_imageFile).fileName();
        createDomTextElement("filename", filename, root, doc);
        createDomTextElement("segmented", "0", root, doc);

        for (const auto &bb : _bndboxList) {
            if (! bb.bbox.isValid()) {
                continue;
            }
            auto object = doc.createElement("object");
            root.appendChild(object);
            createDomTextElement("name", bb.name, object, doc);

            QVariantMap box({{"xmin", bb.bbox.left()}, {"ymin", bb.bbox.top()}, {"xmax", bb.bbox.right()}, {"ymax", bb.bbox.bottom()}});
            auto bndbox = doc.createElement("bndbox");
            object.appendChild(bndbox);
            createDomElement(box, bndbox, doc);

            QString truncated = (bb.truncated) ? "1" : "0";
            QVariantMap meta({{"difficult", "0"}, {"occluded", "0"}, {"truncated", truncated}});
            createDomElement(meta, object, doc);
        }

        Image image(_imageFile);
        QVariantMap isize({{"depth", "3"}, {"width", image.width()}, {"height", image.height()}});
        auto size = doc.createElement("size");
        root.appendChild(size);
        createDomElement(isize, size, doc);
        outData = doc.toString();
        break; }

    default:
        tWarn() << "Invalid target type: " << (int)type;
        break;
    }
    return outData;
}


bool FormatConverter::convertTo(Type type, const QString &dstFilePath) const
{
    QFile outFile(dstFilePath);

    if (outFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QTextStream ts(&outFile);
        ts.setCodec("UTF-8");
        ts << convertTo(type);
        ts.flush();
        outFile.close();
        return true;
    }
    return false;
}


void FormatConverter::parse(const QString &srcFilePath)
{
    _bndboxList.clear();
    QFile boxdata(srcFilePath);

    if (boxdata.open(QFile::ReadOnly)) {
        QTextStream ts(&boxdata);
        ts.setCodec("UTF-8");

        switch (_srcType) {
        case Type::KITTI: {
            QString line;

            while (! (line = ts.readLine()).isNull()) {
                auto items = line.split(QRegExp("\\s+"));

                if (items.count() >= 8) {
                    BoundingBox box;
                    box.name = items[0];
                    box.truncated = items[1].toInt();
                    box.bbox.setTopLeft(QPoint((int)items[4].toFloat(), (int)items[5].toFloat()));
                    box.bbox.setBottomRight(QPoint((int)items[6].toFloat(), (int)items[7].toFloat()));
                    _bndboxList << box;
                }
            }
            break; }

        case Type::PASCAL_VOC:
            break;
        default:
            break;
        }
    }
}
