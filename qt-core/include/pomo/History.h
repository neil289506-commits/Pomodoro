#pragma once
#include <QJsonArray>
#include <QString>
namespace pomo {
// 紀錄每個番茄鐘的成功 / 失敗（JSON Lines，存在使用者資料夾）
class History {
public:
    static void record(int index, bool success, const QString& reason = {});
    static QJsonArray load();
    static QString path();
};
}
