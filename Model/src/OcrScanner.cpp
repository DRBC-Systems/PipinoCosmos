#include "OcrScanner.h"
#include <QDebug>

OcrScanner::OcrScanner() {
    api = new tesseract::TessBaseAPI();
    if (api->Init(nullptr, "eng")) { // "eng" language
        qWarning() << "Could not initialize tesseract.";
    }
}

OcrScanner::~OcrScanner() {
    api->End();
    delete api;
}

QString OcrScanner::scanImage(const QString &filePath) {
    Pix *image = pixRead(filePath.toStdString().c_str());
    if (!image) {
        qWarning() << "Could not open image:" << filePath;
        return "";
    }

    api->SetImage(image);
    char *outText = api->GetUTF8Text();
    QString result = QString::fromUtf8(outText);

    delete[] outText;
    pixDestroy(&image);

    return result;
}
