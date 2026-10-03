#pragma once
#include <QString>
namespace pomo::app {
QString name();     // 顯示名稱（改名只需改這裡）
QString version();
void applyIdentity();  // 設定組織 / 應用名稱，QSettings 與資料夾路徑依賴它
}
