#!/bin/bash
set -e

# =====================[ הגדרות ]=====================
# שני מסכים FHD (1920x1080) — אחד מעל השני
W=1920
H=1080

# מיקומים:
TOP_X=0; TOP_Y=0         # עליון
BOT_X=0; BOT_Y=$H        # תחתון (מתחיל בגובה 1080)

# כתובות:
URL_TOP="http://localhost:5173/"
URL_BOT="http://localhost:5173/create-post"

# WM_CLASS ייחודי לכל חלון (לזיהוי מדויק)
CHROME_CLASS_TOP="KioskTop"
CHROME_CLASS_BOT="KioskBot"

# ספריות נתונים נפרדות (מונע keyring/ערבוב מצב בין החלונות)
BASE_DATA="$HOME/.chrome-kiosk"
TOP_DATA="$BASE_DATA/top"
BOT_DATA="$BASE_DATA/bot"
mkdir -p "$TOP_DATA" "$BOT_DATA"
# ====================================================

# מניעת כיבוי/חיסכון מסך
xset -dpms || true
xset s off || true
xset s noblank || true

# ==== עזר ====
chrome_running(){ pgrep -x chrome >/dev/null || pgrep -x google-chrome >/dev/null; }
wid_valid(){ [ -n "$1" ] && xdotool getwindowname "$1" >/dev/null 2>&1; }

place_fullscreen() {
  local wid="$1" x="$2" y="$3" w="$4" h="$5"
  [ -n "$wid" ] || return 0
  wmctrl -i -r "$wid" -b remove,hidden,shaded,below || true
  xdotool windowmap "$wid" || true
  xdotool windowmove "$wid" "$x" "$y" || true
  xdotool windowsize "$wid" "$w" "$h" || true
  wmctrl -i -r "$wid" -b add,above,sticky,fullscreen || true
}

open_top() {
  google-chrome \
    --app="$URL_TOP" \
    --kiosk \
    --incognito \
    --no-first-run \
    --no-default-browser-check \
    --noerrdialogs \
    --disable-translate \
    --disable-features=Translate,TranslateUI,AutofillServerCommunication \
    --password-store=basic \
    --user-data-dir="$TOP_DATA" \
    --class="$CHROME_CLASS_TOP" \
    >/dev/null 2>&1 &
  sleep 2
  WID_TOP=$(xdotool search --sync --onlyvisible --class "$CHROME_CLASS_TOP" | head -n1)
  place_fullscreen "$WID_TOP" "$TOP_X" "$TOP_Y" "$W" "$H"
}

open_bot() {
  google-chrome \
    --app="$URL_BOT" \
    --kiosk \
    --incognito \
    --no-first-run \
    --no-default-browser-check \
    --noerrdialogs \
    --disable-translate \
    --disable-features=Translate,TranslateUI,AutofillServerCommunication \
    --password-store=basic \
    --user-data-dir="$BOT_DATA" \
    --class="$CHROME_CLASS_BOT" \
    >/dev/null 2>&1 &
  sleep 2
  WID_BOT=$(xdotool search --sync --onlyvisible --class "$CHROME_CLASS_BOT" | head -n1)
  place_fullscreen "$WID_BOT" "$BOT_X" "$BOT_Y" "$W" "$H"
}

restore_win() {
  local wid="$1" x="$2" y="$3" w="$4" h="$5"
  [ -n "$wid" ] || return 0
  wmctrl -i -r "$wid" -b remove,hidden,shaded,below || true
  xdotool windowmap "$wid" || true
  xdotool windowmove "$wid" "$x" "$y" || true
  xdotool windowsize "$wid" "$w" "$h" || true
  wmctrl -i -r "$wid" -b add,above,sticky,fullscreen || true
}

# אם חלון לא תקין (כותרת חסרה/נסגר/ברח), סגור והרם רק אותו מחדש עם ה-URL הנכון
ensure_alive_or_reopen() {
  local wid="$1" which="$2"   # which: top|bot
  if ! wid_valid "$wid"; then
    # סגור שאריות אם יש
    [ -n "$wid" ] && wmctrl -i -c "$wid" || true
    sleep 0.5
    if [ "$which" = "top" ]; then open_top; else open_bot; fi
  fi
}

# ===== פתיחה נקייה: סגור ישן ופתח שני חלונות בדיוק =====
pkill -9 google-chrome chrome 2>/dev/null || true
sleep 1
open_top
open_bot

# ===== לולאת שמירה כל 5 שניות =====
while true; do
  if ! chrome_running; then
    pkill -9 google-chrome chrome 2>/dev/null || true
    sleep 1
    open_top
    open_bot
  else
    ensure_alive_or_reopen "$WID_TOP" "top"
    ensure_alive_or_reopen "$WID_BOT" "bot"
  fi

  restore_win "$WID_TOP" "$TOP_X" "$TOP_Y" "$W" "$H"
  restore_win "$WID_BOT" "$BOT_X" "$BOT_Y" "$W" "$H"

  sleep 5
done
