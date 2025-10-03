#include "OcrScanner.h"
#include <QDebug>

OcrScanner::OcrScanner() {
    tess = new tesseract::TessBaseAPI();
    if (tess->Init(nullptr, "eng")) {
        qDebug() << "Tesseract initialization failed";
    }
}

OcrScanner::~OcrScanner() {
    if (tess) {
        tess->End();
        delete tess;
    }
}

QString OcrScanner::scanImage(const QString &filePath) {
    Pix *image = pixRead(filePath.toStdString().c_str());
    if (!image) {
        return "Error: Could not read image";
    }

    tess->SetImage(image);
    char *outText = tess->GetUTF8Text();
    QString result(outText);
    delete[] outText;
    pixDestroy(&image);

    return result.trimmed();
}

QString OcrScanner::getTextFromImage(const QString &filePath) {
    return scanImage(filePath);
}



