#pragma once
#include <QString>
namespace pomo::appentry {
// 放行清單的一筆可以是「執行檔完整路徑」、「程式名稱」或 macOS 的 .app 路徑。
QString idOf(const QString& entry);     // 與前景偵測回報的 ID 相同：檔名、小寫、去掉 .app
bool launch(const QString& entry);      // 啟動該程式
}
