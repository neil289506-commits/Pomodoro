#pragma once
#include <QObject>
#include <memory>
class QTextToSpeech;
namespace pomo {
class IVolume { public: virtual ~IVolume() = default; virtual bool setPercent(int percent) = 0; };
// 由各桌面平台目錄實作（Windows / macOS / Linux）
std::unique_ptr<IVolume> createPlatformVolume();
// 語音提醒前先把系統音量拉到 50%
class Announcer : public QObject {
    Q_OBJECT
public:
    explicit Announcer(QObject* parent = nullptr);
    void say(const QString& text);
private:
    std::unique_ptr<IVolume> vol_;
    std::unique_ptr<QTextToSpeech> tts_;
};
}
