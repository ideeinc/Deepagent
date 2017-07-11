#include "logics/dbdetectionconverter.h"

REGISTER_DBCONVERTER(Detection, DbDetectionConverter);


DbDetectionConverter::DbDetectionConverter(const CaffeData::Category& layout, const QString& sourceDir)
    : DbConverter(layout, sourceDir)
{
}

void
DbDetectionConverter::convertTo(const QString& destinationDir)
{
    tDebug() << "Not yet implementation";
    tDebug() << "> source directory: " << sourceDir();
    tDebug() << "> destination directory: " << destinationDir;
    tDebug() << "> backend: " << backendName();
    tDebug() << "> layout: " << layoutName();
    tDebug() << "> image size: " << imageSize().width() << " x " << imageSize().height();
}
