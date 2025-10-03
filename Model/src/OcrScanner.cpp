#include "OcrScanner.h"
#include <QDebug>
#include <QDir>
#include <QCoreApplication>

OcrScanner::OcrScanner() {
    tess = new tesseract::TessBaseAPI();
    
    // Try to find tessdata directory
    QStringList tessdataPaths = {
        "C:/Program Files/Tesseract-OCR/tessdata",
        QCoreApplication::applicationDirPath() + "/tessdata",
        QDir::currentPath() + "/tessdata",
        "../tessdata",
        "tessdata"
    };
    
    bool initialized = false;
    for (const QString& path : tessdataPaths) {
        QDir dir(path);
        if (dir.exists()) {
            qDebug() << "Trying tessdata path:" << path;
            if (tess->Init(path.toStdString().c_str(), "eng") == 0) {
                qDebug() << "Tesseract initialized successfully with path:" << path;
                initialized = true;
                break;
            }
        }
    }
    
    if (!initialized) {
        qWarning() << "Could not initialize tesseract. Trying with nullptr...";
        if (tess->Init(nullptr, "eng") != 0) {
            qWarning() << "Tesseract initialization failed completely!";
            qWarning() << "Please ensure tessdata folder is in one of these locations:";
            for (const QString& path : tessdataPaths) {
                qWarning() << "  -" << path;
            }
        }
    }
}

OcrScanner::~OcrScanner() {
    if (tess) {
        tess->End();
        delete tess;
    }
}

QString OcrScanner::scanImage(const QString &filePath) {
    // Check if API is properly initialized
    if (!tess) {
        qWarning() << "Tesseract API is not initialized!";
        return "OCR Error: Tesseract not initialized";
    }
    
    qDebug() << "Scanning image:" << filePath;
    
    Pix *image = pixRead(filePath.toStdString().c_str());
    if (!image) {
        qWarning() << "Could not open image:" << filePath;
        return "OCR Error: Could not open image file";
    }

    tess->SetImage(image);
    char *outText = tess->GetUTF8Text();
    
    if (!outText) {
        qWarning() << "OCR failed to extract text from image";
        pixDestroy(&image);
        return "OCR Error: Failed to extract text";
    }
    
    QString result = QString::fromUtf8(outText);
    qDebug() << "OCR Result:" << result;

    delete[] outText;
    pixDestroy(&image);

    return result.isEmpty() ? "OCR Error: No text detected in image" : result;
}

QString OcrScanner::getTextFromImage(const QString &filePath) {
    return scanImage(filePath);
}
