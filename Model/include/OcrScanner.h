#pragma once
#include <QString>
#include <tesseract/baseapi.h>
#include <allheaders.h>

class OcrScanner {
public:
    OcrScanner();
    ~OcrScanner();
    QString scanImage(const QString &filePath);

private:
    tesseract::TessBaseAPI *api;
};
