#!/usr/bin/env python3
"""產生 i18n/strings.json（所有平台共用的多語系字串來源）。
欄位順序：key, en, zh-TW, zh-CN, ja, ko, es。新增語言：在 LANGS 加一個欄位並補齊每一列。
用法：python3 i18n/build_catalog.py"""
import json, re, sys, os
LANGS = [("en","English"),("zh-TW","繁體中文"),("zh-CN","简体中文"),("ja","日本語"),("ko","한국어"),("es","Español")]
R = [
("tab.timer","Timer","計時","计时","タイマー","타이머","Temporizador"),
("tab.history","History","紀錄","记录","履歴","기록","Historial"),
("tab.settings","Settings","設定","设置","設定","설정","Ajustes"),
("phase.idle","Ready","準備就緒","准备就绪","準備完了","준비됨","Listo"),
("phase.prep","Get ready","準備中","准备中","準備中","준비 중","Prepárate"),
("phase.work","Focus","專注中","专注中","集中","집중","Enfoque"),
("phase.rest","Break","休息中","休息中","休憩","휴식","Descanso"),
("phase.finished","All done","全部完成","全部完成","すべて完了","모두 완료","Todo listo"),
("phase.voided","Voided","已作廢","已作废","無効","무효","Anulado"),
("timer.pomodoros","Pomodoros","番茄鐘數量","番茄钟数量","ポモドーロ数","포모도로 수","Pomodoros"),
("timer.start","Start","開始","开始","開始","시작","Iniciar"),
("timer.start_hint","A 3-minute countdown to get ready comes first","先倒數 3 分鐘準備，再開始專注","先倒数 3 分钟准备，再开始专注","まず3分間の準備カウントダウンがあります","먼저 3분간 준비 카운트다운이 진행됩니다","Primero hay una cuenta atrás de 3 minutos para prepararte"),
("timer.stop","Stop","停止","停止","停止","중지","Detener"),
("timer.launch","Open allowed app (%1)","開啟放行 App（%1）","打开放行应用（%1）","許可したアプリを開く（%1）","허용된 앱 열기(%1)","Abrir app permitida (%1)"),
("timer.launch_plain","Open allowed app","開啟放行 App","打开放行应用","許可したアプリを開く","허용된 앱 열기","Abrir app permitida"),
("timer.round","Pomodoro %1 of %2","第 %1 / %2 個番茄鐘","第 %1 / %2 个番茄钟","ポモドーロ %1 / %2","포모도로 %1 / %2","Pomodoro %1 de %2"),
("timer.idle_hint","Pick how many pomodoros to do","選擇要做幾個番茄鐘","选择要做几个番茄钟","ポモドーロの数を選択","포모도로 횟수를 선택하세요","Elige cuántos pomodoros hacer"),
("timer.prep_hint","Tidy your workspace and get ready","整理好工作環境，準備開始","整理好工作环境，准备开始","作業環境を整えて、準備しましょう","작업 환경을 정리하고 준비하세요","Ordena tu espacio y prepárate"),
("timer.rest_hint","Step away and rest your mind","離開座位，讓大腦休息一下","离开座位，让大脑休息一下","席を離れて、頭を休めましょう","자리에서 벗어나 잠시 쉬세요","Aléjate y descansa la mente"),
("timer.done_hint","Great work! All pomodoros completed","太棒了！所有番茄鐘都完成了","太棒了！所有番茄钟都完成了","お疲れさま！すべて完了しました","잘했어요! 모든 포모도로를 완료했어요","¡Buen trabajo! Has completado todos los pomodoros"),
("timer.voided_hint","This pomodoro was voided","這個番茄鐘已作廢","这个番茄钟已作废","このポモドーロは無効になりました","이 포모도로는 무효 처리되었어요","Este pomodoro se ha anulado"),
("guard.idle","Focus Guard: standby","專注守護：待命","专注守护：待命","フォーカスガード：待機中","집중 가드: 대기 중","Guardia de enfoque: en espera"),
("guard.leaves","Focus Guard: left %1 / %2 times","專注守護：已離開 %1 / %2 次","专注守护：已离开 %1 / %2 次","フォーカスガード：離脱 %1 / %2 回","집중 가드: 이탈 %1 / %2회","Guardia de enfoque: saliste %1 / %2 veces"),
("guard.limited","Focus Guard limitation: %1","專注守護限制：%1","专注守护限制：%1","フォーカスガードの制限：%1","집중 가드 제한: %1","Limitación de la guardia: %1"),
("watcher.no_x11","Only X11 foreground detection is supported (Wayland or no DISPLAY)","僅支援 X11 前景偵測（Wayland 或沒有 DISPLAY）","仅支持 X11 前台检测（Wayland 或没有 DISPLAY）","前面アプリの検出は X11 のみ対応です（Wayland または DISPLAY なし）","포그라운드 앱 감지는 X11만 지원합니다(Wayland 또는 DISPLAY 없음)","Solo se admite la detección en primer plano con X11 (Wayland o sin DISPLAY)"),
("watcher.x11_connect","Cannot connect to X11; Wayland needs extra compositor support","無法連線 X11；Wayland 需要額外的 compositor 支援","无法连接 X11；Wayland 需要额外的 compositor 支持","X11 に接続できません。Wayland には compositor 側の対応が必要です","X11에 연결할 수 없습니다. Wayland는 추가 컴포지터 지원이 필요합니다","No se puede conectar con X11; Wayland necesita soporte adicional del compositor"),
("watcher.x11_ewmh","X11 does not support _NET_ACTIVE_WINDOW","X11 不支援 _NET_ACTIVE_WINDOW","X11 不支持 _NET_ACTIVE_WINDOW","X11 が _NET_ACTIVE_WINDOW に対応していません","X11이 _NET_ACTIVE_WINDOW를 지원하지 않습니다","X11 no admite _NET_ACTIVE_WINDOW"),
("lock.failed","Hard lock failed: %1","硬鎖失敗：%1","硬锁失败：%1","ハードロックに失敗：%1","하드 잠금 실패: %1","Falló el bloqueo duro: %1"),
("lock.paused","Hard lock paused while an allowed app is in use. Leave it or press %1 to lock again.","已暫停硬鎖：放行 App 使用中。離開該 App 或再按 %1 就會重新鎖上。","已暂停硬锁：放行应用使用中。离开该应用或再按 %1 就会重新锁上。","ハードロック一時停止中：許可したアプリを使用中です。アプリを離れるか %1 でもう一度ロックします。","하드 잠금 일시 중지: 허용된 앱 사용 중입니다. 앱을 벗어나거나 %1을(를) 누르면 다시 잠깁니다.","Bloqueo duro en pausa mientras usas una app permitida. Sal de ella o pulsa %1 para volver a bloquear."),
("lock.hotkey_line","Shortcut: %1","熱鍵：%1","快捷键：%1","ショートカット：%1","단축키: %1","Atajo: %1"),
("lock.desc.windows","Hard lock (keyboard hook): Win key, Alt+Tab, Alt+Esc, Alt+F4, Ctrl+Esc and Ctrl+Shift+Esc are blocked. Ctrl+Alt+Del cannot be blocked.","硬鎖（鍵盤 hook）：Win 鍵、Alt+Tab、Alt+Esc、Alt+F4、Ctrl+Esc、Ctrl+Shift+Esc 已攔截。無法攔截 Ctrl+Alt+Del。","硬锁（键盘 hook）：Win 键、Alt+Tab、Alt+Esc、Alt+F4、Ctrl+Esc、Ctrl+Shift+Esc 已拦截。无法拦截 Ctrl+Alt+Del。","ハードロック（キーボードフック）：Win キー、Alt+Tab、Alt+Esc、Alt+F4、Ctrl+Esc、Ctrl+Shift+Esc をブロック中。Ctrl+Alt+Del はブロックできません。","하드 잠금(키보드 후크): Win 키, Alt+Tab, Alt+Esc, Alt+F4, Ctrl+Esc, Ctrl+Shift+Esc를 차단합니다. Ctrl+Alt+Del은 차단할 수 없습니다.","Bloqueo duro (gancho de teclado): se bloquean la tecla Win, Alt+Tab, Alt+Esc, Alt+F4, Ctrl+Esc y Ctrl+Shift+Esc. Ctrl+Alt+Supr no se puede bloquear."),
("lock.desc.macos","Hard lock (kiosk mode): Cmd+Tab, the Dock, the menu bar, Force Quit and log out are disabled. The power button cannot be blocked.","硬鎖（Kiosk 模式）：Cmd+Tab、Dock、選單列、強制結束（Cmd+Option+Esc）、登出已停用。無法攔截電源鍵。","硬锁（Kiosk 模式）：Cmd+Tab、Dock、菜单栏、强制退出（Cmd+Option+Esc）、登出已停用。无法拦截电源键。","ハードロック（キオスクモード）：Cmd+Tab、Dock、メニューバー、強制終了、ログアウトを無効化しています。電源ボタンはブロックできません。","하드 잠금(키오스크 모드): Cmd+Tab, Dock, 메뉴 막대, 강제 종료, 로그아웃이 비활성화됩니다. 전원 버튼은 차단할 수 없습니다.","Bloqueo duro (modo quiosco): Cmd+Tab, el Dock, la barra de menús, Forzar salida y cerrar sesión están desactivados. El botón de encendido no se puede bloquear."),
("lock.desc.linux","Hard lock (X11 keyboard grab): window-manager shortcuts such as Alt+Tab and Super are blocked. Ctrl+Alt+F1–F7 and the power button cannot be blocked.","硬鎖（X11 鍵盤獨佔）：Alt+Tab、Super 等切換快捷鍵已攔截。無法攔截 Ctrl+Alt+F1~F7 切換終端機與電源鍵。","硬锁（X11 键盘独占）：Alt+Tab、Super 等切换快捷键已拦截。无法拦截 Ctrl+Alt+F1~F7 切换终端和电源键。","ハードロック（X11 キーボード占有）：Alt+Tab や Super などのウィンドウ切替ショートカットをブロック中。Ctrl+Alt+F1〜F7 と電源ボタンはブロックできません。","하드 잠금(X11 키보드 독점): Alt+Tab, Super 등 창 전환 단축키를 차단합니다. Ctrl+Alt+F1~F7과 전원 버튼은 차단할 수 없습니다.","Bloqueo duro (captura de teclado X11): se bloquean atajos como Alt+Tab y Super. Ctrl+Alt+F1–F7 y el botón de encendido no se pueden bloquear."),
("lock.err.wayland","Wayland does not allow apps to block system shortcuts: hard lock is unavailable, using always-on-top fullscreen only","Wayland 不允許攔截系統快捷鍵：無法硬鎖，只能使用全螢幕置頂","Wayland 不允许拦截系统快捷键：无法硬锁，只能使用全屏置顶","Wayland ではアプリがシステムショートカットをブロックできません。ハードロックは使えず、最前面の全画面のみになります","Wayland는 앱이 시스템 단축키를 차단하는 것을 허용하지 않습니다. 하드 잠금을 쓸 수 없어 항상 위 전체 화면만 사용합니다","Wayland no permite bloquear los atajos del sistema: el bloqueo duro no está disponible, solo pantalla completa siempre visible"),
("lock.err.x11","Cannot connect to X11, so hard lock is unavailable","無法連線 X11，無法硬鎖","无法连接 X11，无法硬锁","X11 に接続できないため、ハードロックは使えません","X11에 연결할 수 없어 하드 잠금을 쓸 수 없습니다","No se puede conectar con X11, el bloqueo duro no está disponible"),
("lock.err.grab","Could not grab the keyboard (X11 error %1). Another program may hold it.","無法取得鍵盤獨佔（X11 錯誤碼 %1，可能有其他程式已獨佔鍵盤）","无法取得键盘独占（X11 错误码 %1，可能有其他程序已独占键盘）","キーボードを占有できません（X11 エラー %1）。他のプログラムが占有している可能性があります。","키보드를 독점할 수 없습니다(X11 오류 %1). 다른 프로그램이 점유 중일 수 있습니다.","No se pudo capturar el teclado (error X11 %1). Puede que otro programa lo esté usando."),
("lock.err.hook","Could not install the keyboard hook (error %1)","無法安裝鍵盤攔截（錯誤碼 %1）","无法安装键盘拦截（错误码 %1）","キーボードフックを設定できません（エラー %1）","키보드 후크를 설치할 수 없습니다(오류 %1)","No se pudo instalar el gancho de teclado (error %1)"),
("confirm.title","Stop now?","現在停止？","现在停止？","今すぐ停止しますか？","지금 중지할까요?","¿Detener ahora?"),
("confirm.work","This pomodoro will be voided and cannot be resumed.","這個番茄鐘將作廢，無法接續。","这个番茄钟将作废，无法继续。","このポモドーロは無効になり、再開できません。","이 포모도로는 무효 처리되며 이어서 할 수 없습니다.","Este pomodoro se anulará y no se podrá reanudar."),
("confirm.rest","This ends the whole session.","這會結束整個流程。","这会结束整个流程。","セッション全体が終了します。","전체 세션이 종료됩니다.","Esto termina toda la sesión."),
("confirm.keep","Keep going","繼續專注","继续专注","続ける","계속하기","Continuar"),
("confirm.stop","Stop","停止","停止","停止","중지","Detener"),
("history.completed","Completed","已完成","已完成","完了","완료","Completados"),
("history.voided","Voided","已作廢","已作废","無効","무효","Anulados"),
("history.best_streak","Best streak","最佳連勝","最佳连胜","最高連続","최고 연속","Mejor racha"),
("history.index","Pomodoro #%1","番茄鐘 #%1","番茄钟 #%1","ポモドーロ #%1","포모도로 #%1","Pomodoro #%1"),
("history.empty","No pomodoros yet.\nStart your first one from the Timer tab.","還沒有紀錄。\n到「計時」開始你的第一個番茄鐘吧。","还没有记录。\n到“计时”开始你的第一个番茄钟吧。","まだ記録がありません。\n「タイマー」で最初のポモドーロを始めましょう。","아직 기록이 없습니다.\n타이머 탭에서 첫 포모도로를 시작해 보세요.","Aún no hay pomodoros.\nEmpieza el primero desde la pestaña Temporizador."),
("reason.manual","Stopped manually","手動停止","手动停止","手動で停止","직접 중지","Detenido manualmente"),
("reason.leaves","Left the app more than %1 times","離開視窗超過 %1 次","离开窗口超过 %1 次","アプリを %1 回より多く離れました","앱을 %1회보다 많이 벗어났습니다","Saliste de la app más de %1 veces"),
("settings.language","Language","語言","语言","言語","언어","Idioma"),
("settings.language.auto","Automatic (system)","自動（跟隨系統）","自动（跟随系统）","自動（システムに合わせる）","자동(시스템 설정)","Automático (sistema)"),
("settings.apps.title","Allowed apps","放行的 App","放行的应用","許可するアプリ","허용된 앱","Apps permitidas"),
("settings.apps.hint","Apps you may use during a focus session. Pick the program with Browse so the shortcut can open it.","專注中可以使用的 App。用「瀏覽」選擇程式，才能用熱鍵直接開啟。","专注中可以使用的应用。用“浏览”选择程序，才能用快捷键直接打开。","集中中に使ってよいアプリです。「参照」で選ぶと、ショートカットから直接開けます。","집중 중에 사용할 수 있는 앱입니다. 찾아보기로 프로그램을 선택하면 단축키로 바로 열 수 있습니다.","Apps que puedes usar durante una sesión. Elígelas con Examinar para poder abrirlas con el atajo."),
("settings.sites.title","Allowed websites","放行的網站","放行的网站","許可するサイト","허용된 웹사이트","Sitios permitidos"),
("settings.sites.hint","Website blocking is not available yet. This list is saved for a future version.","網站封鎖功能尚未提供，這份清單會先保存，供日後版本使用。","网站拦截功能尚未提供，这份清单会先保存，供日后版本使用。","サイトのブロック機能はまだありません。このリストは将来のバージョンのために保存されます。","웹사이트 차단 기능은 아직 제공되지 않습니다. 이 목록은 이후 버전을 위해 저장됩니다.","El bloqueo de sitios aún no está disponible. Esta lista se guarda para una versión futura."),
("settings.add","Add","新增","添加","追加","추가","Añadir"),
("settings.remove","Remove selected","移除選取","移除所选","選択を削除","선택 삭제","Quitar selección"),
("settings.browse","Browse…","瀏覽…","浏览…","参照…","찾아보기…","Examinar…"),
("settings.placeholder.app","Program path or name","程式路徑或名稱","程序路径或名称","プログラムのパスまたは名前","프로그램 경로 또는 이름","Ruta o nombre del programa"),
("settings.placeholder.site","example.com","example.com","example.com","example.com","example.com","example.com"),
("settings.hotkey","Shortcut to open allowed apps during focus: %1","專注中開啟放行 App 的熱鍵：%1","专注中打开放行应用的快捷键：%1","集中中に許可したアプリを開くショートカット：%1","집중 중 허용된 앱을 여는 단축키: %1","Atajo para abrir apps permitidas durante el enfoque: %1"),
("settings.about","About","關於","关于","このアプリについて","정보","Acerca de"),
("settings.version","Version %1","版本 %1","版本 %1","バージョン %1","버전 %1","Versión %1"),
("settings.permissions","Permissions","權限","权限","権限","권한","Permisos"),
("settings.perm.usage","Usage access (lets the guard see the foreground app)","使用情況存取（讓專注守護知道前景 App）","使用情况访问（让专注守护知道前台应用）","使用状況へのアクセス（フォーカスガードが前面のアプリを把握するため）","사용 정보 접근(집중 가드가 포그라운드 앱을 확인)","Acceso de uso (permite a la guardia ver la app en primer plano)"),
("settings.perm.dnd","Do Not Disturb access (silences notifications while focusing)","勿擾權限（專注時靜音通知）","勿扰权限（专注时静音通知）","おやすみモードへのアクセス（集中中の通知を消音）","방해 금지 접근(집중 중 알림 무음)","Acceso a No molestar (silencia notificaciones al enfocarte)"),
("settings.perm.open","Open settings","開啟設定","打开设置","設定を開く","설정 열기","Abrir ajustes"),
("settings.ios_limit","iOS does not let apps see other apps or block them. The guard only detects when you leave this app.","iOS 不允許 App 偵測或封鎖其他 App，專注守護只能偵測你是否離開本 App。","iOS 不允许应用检测或拦截其他应用，专注守护只能检测你是否离开本应用。","iOS ではアプリが他のアプリを検出・ブロックできません。フォーカスガードはこのアプリを離れたことだけ検出します。","iOS는 앱이 다른 앱을 감지하거나 차단하도록 허용하지 않습니다. 집중 가드는 이 앱을 벗어났는지만 감지합니다.","iOS no permite que las apps vean o bloqueen otras apps. La guardia solo detecta cuando sales de esta app."),
("about.tagline","A focus timer inspired by the Pomodoro Technique","受番茄工作法啟發的專注計時器","受番茄工作法启发的专注计时器","ポモドーロ・テクニックにヒントを得た集中タイマー","포모도로 기법에서 영감을 받은 집중 타이머","Un temporizador de enfoque inspirado en la Técnica Pomodoro"),
("launcher.title","Allowed apps","放行的 App","放行的应用","許可したアプリ","허용된 앱","Apps permitidas"),
("launcher.pick","Choose an app to open (%1)","選擇要開啟的 App（%1）","选择要打开的应用（%1）","開くアプリを選択（%1）","열 앱을 선택하세요(%1)","Elige una app para abrir (%1)"),
("launcher.open","Open","開啟","打开","開く","열기","Abrir"),
("launcher.empty","No allowed apps yet.\nAdd some in Settings before starting a session.","尚未設定放行的 App。\n請在開始專注前，到「設定」分頁新增。","尚未设置放行的应用。\n请在开始专注前，到“设置”页新增。","許可したアプリがまだありません。\n開始前に「設定」で追加してください。","허용된 앱이 아직 없습니다.\n세션을 시작하기 전에 설정에서 추가하세요.","Aún no hay apps permitidas.\nAñade alguna en Ajustes antes de empezar."),
("launcher.failed","Could not start — use the full path","無法啟動，請改用完整路徑","无法启动，请改用完整路径","起動できません。フルパスを使ってください","시작할 수 없습니다. 전체 경로를 사용하세요","No se pudo iniciar: usa la ruta completa"),
("speech.prep","Get ready. Focus starts soon.","準備開始，請整理好工作環境","准备开始，请整理好工作环境","準備を始めましょう。まもなく集中タイムです。","준비하세요. 곧 집중 시간이 시작됩니다.","Prepárate. El enfoque empieza pronto."),
("speech.work","Focus time. Let's go.","開始專注","开始专注","集中タイムの開始です。","집중 시간입니다. 시작하세요.","Hora de enfocarse. Vamos."),
("speech.rest","Pomodoro complete. Take a break.","番茄鐘完成，請休息","番茄钟完成，请休息","ポモドーロ完了。休憩しましょう。","포모도로 완료. 잠시 쉬세요.","Pomodoro completado. Toma un descanso."),
("speech.finished","All done. Great work.","全部完成","全部完成","すべて完了しました。お疲れさま。","모두 완료했습니다. 수고하셨어요.","Todo listo. Buen trabajo."),
("speech.voided","This pomodoro was voided.","番茄鐘已作廢","番茄钟已作废","このポモドーロは無効になりました。","이 포모도로는 무효 처리되었습니다.","Este pomodoro se ha anulado."),
]
def main():
    strings = {}
    for row in R:
        assert len(row) == 1 + len(LANGS), f"欄位數不符：{row[0]}"
        strings[row[0]] = {code: row[1 + i] for i, (code, _) in enumerate(LANGS)}
    ph = lambda s: sorted(re.findall(r"%\d", s))
    for k, v in strings.items():          # 檢查：每個語言的 %1 %2 佔位符要與英文一致
        for code in v:
            if ph(v[code]) != ph(v["en"]): sys.exit(f"佔位符不一致：{k} [{code}]")
    out = {"languages": [{"code": c, "name": n} for c, n in LANGS], "strings": strings}
    path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "strings.json")
    json.dump(out, open(path, "w", encoding="utf-8"), ensure_ascii=False, indent=1)
    print(f"{len(strings)} 個字串 × {len(LANGS)} 種語言 → {path}")
main()
