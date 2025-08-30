// imgv_qt.cpp - Qt5 version of the image viewer
// Build with: g++ imgv_qt.cpp -o imgv_qt $(pkg-config --cflags --libs Qt5Widgets)

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QImageReader>
#include <QDir>
#include <QFileInfo>
#include <QKeyEvent>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QScreen>
#include <QDebug>
#include <vector>
#include <algorithm>

class ImageViewer : public QWidget {
public:
    ImageViewer(const QString &filepath, QWidget *parent = nullptr)
        : QWidget(parent), label(new QLabel(this)), dragging(false) {
        setWindowTitle("imgv - Qt");
        setWindowFlags(windowFlags() | Qt::Window);
        setAttribute(Qt::WA_DeleteOnClose);
        label->setAlignment(Qt::AlignCenter);
        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->addWidget(label);
        layout->setContentsMargins(0,0,0,0);
        setLayout(layout);
        loadImageList(filepath);
        loadImage(currentIndex);
    }

protected:
    void keyPressEvent(QKeyEvent *event) override {
        switch (event->key()) {
            case Qt::Key_Escape:
            case Qt::Key_Q:
                close();
                break;
            case Qt::Key_Right:
            case Qt::Key_Space:
                nextImage();
                break;
            case Qt::Key_Left:
            case Qt::Key_Backspace:
                prevImage();
                break;
            default:
                QWidget::keyPressEvent(event);
        }
    }

    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            dragging = true;
            dragStartPos = event->globalPos();
            windowStartPos = frameGeometry().topLeft();
        }
    }
    void mouseMoveEvent(QMouseEvent *event) override {
        if (dragging) {
            QPoint delta = event->globalPos() - dragStartPos;
            move(windowStartPos + delta);
        }
    }
    void mouseReleaseEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            dragging = false;
        }
    }

private:
    QLabel *label;
    std::vector<QString> imageFiles;
    int currentIndex = 0;
    QString currentDir;
    bool dragging;
    QPoint dragStartPos, windowStartPos;

    bool isImageFile(const QString &filename) {
        static const QStringList exts = {".jpg", ".jpeg", ".png", ".bmp", ".tga", ".gif"};
        QString ext = QFileInfo(filename).suffix().toLower();
        return exts.contains("." + ext);
    }

    void loadImageList(const QString &filepath) {
        QFileInfo fi(filepath);
        currentDir = fi.absolutePath();
        QDir dir(currentDir);
        QStringList files = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
        imageFiles.clear();
        for (const QString &f : files) {
            if (isImageFile(f))
                imageFiles.push_back(dir.absoluteFilePath(f));
        }
        std::sort(imageFiles.begin(), imageFiles.end(), [](const QString &a, const QString &b) {
            return a.toLower() < b.toLower();
        });
        currentIndex = std::distance(imageFiles.begin(), std::find(imageFiles.begin(), imageFiles.end(), fi.absoluteFilePath()));
        if (currentIndex < 0 || currentIndex >= (int)imageFiles.size())
            currentIndex = 0;
    }

    void loadImage(int idx) {
        if (imageFiles.empty()) return;
        if (idx < 0 || idx >= (int)imageFiles.size()) idx = 0;
        QImageReader reader(imageFiles[idx]);
        QImage img = reader.read();
        if (img.isNull()) {
            label->setText("Cannot load image: " + imageFiles[idx]);
            return;
        }
        // Resize window to fit image, but not larger than 90% of screen
        QScreen *screen = QGuiApplication::primaryScreen();
        QSize maxSize = screen->availableGeometry().size() * 0.9;
        QSize imgSize = img.size();
        if (imgSize.width() > maxSize.width() || imgSize.height() > maxSize.height()) {
            img = img.scaled(maxSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            imgSize = img.size();
        }
        setFixedSize(imgSize);
        label->setPixmap(QPixmap::fromImage(img));
        setWindowTitle(QString("imgv - %1 (%2x%3)").arg(QFileInfo(imageFiles[idx]).fileName()).arg(img.width()).arg(img.height()));
    }

    void nextImage() {
        if (imageFiles.empty()) return;
        currentIndex = (currentIndex + 1) % imageFiles.size();
        loadImage(currentIndex);
    }
    void prevImage() {
        if (imageFiles.empty()) return;
        currentIndex = (currentIndex - 1 + imageFiles.size()) % imageFiles.size();
        loadImage(currentIndex);
    }
};

#include <iostream>
int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <image_path>\n";
        return 1;
    }
    QApplication app(argc, argv);
    ImageViewer viewer(argv[1]);
    viewer.show();
    return app.exec();
}

