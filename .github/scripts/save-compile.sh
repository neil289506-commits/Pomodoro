#!/usr/bin/env bash
# 把編譯產物保存到 repo 的 compile/<名稱>/ 並推送到 main。
# 用法：save-compile.sh <名稱> <檔案或資料夾>...
# 只由 CI 在 main 的 push 上呼叫；用 GITHUB_TOKEN 推送不會再次觸發 workflow，所以不會無限循環。
set -euo pipefail
NAME="$1"; shift

stage="$(mktemp -d)"
copied=0
for p in "$@"; do
  if [ -e "$p" ]; then cp -R "$p" "$stage/"; copied=1; else echo "略過不存在的路徑：$p"; fi
done
[ "$copied" = 1 ] || { echo "::error::找不到任何要保存的產物"; exit 1; }

cat > "$stage/BUILD_INFO.txt" <<INFO
commit: ${GITHUB_SHA}
run:    ${GITHUB_SERVER_URL}/${GITHUB_REPOSITORY}/actions/runs/${GITHUB_RUN_ID}
time:   $(date -u +%Y-%m-%dT%H:%M:%SZ)
target: ${NAME}
INFO

git config user.name  "github-actions[bot]"
git config user.email "41898282+github-actions[bot]@users.noreply.github.com"
git config core.autocrlf false

# 五個 job 會同時推送，衝突時重試
for try in 1 2 3 4 5 6; do
  git fetch -q origin main
  git checkout -q -B compile-publish origin/main
  rm -rf "compile/${NAME}"
  mkdir -p "compile/${NAME}"
  cp -R "$stage/." "compile/${NAME}/"
  git add -A compile
  if git diff --cached --quiet; then echo "沒有變更"; exit 0; fi
  git commit -q -m "chore: 更新編譯產物 ${NAME} (${GITHUB_SHA:0:7}) [skip ci]"
  if git push -q origin HEAD:main; then echo "已保存到 compile/${NAME}"; exit 0; fi
  echo "推送衝突，重試 ${try}"; sleep $((RANDOM % 6 + 2))
done
echo "::error::多次重試仍無法推送 compile/${NAME}"; exit 1
