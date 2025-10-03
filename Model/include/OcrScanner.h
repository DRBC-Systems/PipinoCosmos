#ifndef OCRSCANNER_H
#define OCRSCANNER_H

#include <QString>
#include <tesseract/baseapi.h>
#include <allheaders.h>

class OcrScanner {
public:
    OcrScanner();
    ~OcrScanner();

    QString scanImage(const QString &filePath);

    // Getter function for external use
    QString getTextFromImage(const QString &filePath);

private:
    tesseract::TessBaseAPI *tess;  // Make sure this exists
};

#endif // OCRSCANNER_H
