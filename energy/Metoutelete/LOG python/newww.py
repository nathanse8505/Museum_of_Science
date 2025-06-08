import pandas as pd
import re
from datetime import datetime
import matplotlib.pyplot as plt

# === PARAMÈTRES ===
file_path = "graph/april/LOG_april.TXT"
offset_ms = 20 * 60 * 60 * 1000  # Décalage de 20h
pattern = re.compile(r"(\d+)\s*ms\s*;", re.IGNORECASE)
motor_keywords = ["Motor activated", "Motor"]
MONTH = 6
YEAR = 2025
# === LECTURE ET PARSING ===
time_ms_raw = []
time_ms_offset = []
button_list = []
motor_status_list = []

with open(file_path, "r", encoding="utf-8") as f:
    lines = f.readlines()

for line in lines:
    match = pattern.search(line)
    if match:
        raw_time = int(match.group(1))
        time_ms = raw_time + offset_ms
        activated = any(keyword.lower() in line.lower() for keyword in motor_keywords)

        time_ms_raw.append(raw_time)
        time_ms_offset.append(time_ms)
        motor_status_list.append("YES" if activated else "NO")

# === DATAFRAME PRINCIPAL ===
df = pd.DataFrame({
    "Time_ms_raw": time_ms_raw,
    "Time_ms": time_ms_offset,  # avec offset
    "Motor_Activated": motor_status_list
}).sort_values("Time_ms").reset_index(drop=True)

# === DISTRIBUTION PAR JOUR ===
total_pushes = len(df)
average_per_day = total_pushes // 30
remainder = total_pushes % 30

dates = [datetime(2025, MONTH, day).date() for day in range(1, 31)]
date_labels = []
for i in range(30):
    count = average_per_day + (1 if i < remainder else 0)
    date_labels.extend([dates[i]] * count)

df["Date_Calculée"] = date_labels

# === RÉSUMÉ JOURNALIER ===
grouped = df.groupby(["Date_Calculée", "Motor_Activated"]).size().unstack(fill_value=0)
grouped = grouped.rename(columns={"NO": "NO ACTION", "YES": "MOTOR ON"})

total_avec_uv = grouped.get("MOTOR ON", pd.Series()).sum()
total_sans_uv = grouped.get("NO ACTION", pd.Series()).sum()
total_all = total_avec_uv + total_sans_uv

df_resume = pd.DataFrame({
    "Date": grouped.index,
    "MOTOR ON": grouped.get("MOTOR ON", 0),
    "NO ACTION": grouped.get("NO ACTION", 0),
    "Total with Motor": [total_avec_uv] + [None] * (len(grouped) - 1),
    "Total NO ACTION": [total_sans_uv] + [None] * (len(grouped) - 1),
    "Total": [total_all] + [None] * (len(grouped) - 1)
})

# === EXPORT EXCEL À 2 ONGLET ===
with pd.ExcelWriter(f"df_{MONTH}.xlsx") as writer:
    df.to_excel(writer, sheet_name="Raw Data", index=False)
    df_resume.to_excel(writer, sheet_name="Résumé per day", index=False)

print(f"✅ Excel avec 2 onglets créé : df_{MONTH}.xlsx")

# === GRAPHIQUE ===
plt.figure(figsize=(14, 6))
df_resume.set_index("Date")["MOTOR ON"].plot(kind="bar", color="orange", width=0.8)
plt.title("Number of pushes per day (motor activate)")
plt.xlabel("Date (avril 2025)")
plt.ylabel("Number of pushes with motor ON")
plt.xticks(rotation=45, ha='right')
plt.tight_layout()
plt.grid(axis='y')
plt.savefig(f"graph_{MONTH}_{YEAR}.png")
# plt.show()

print(f"📊 Graphique sauvegardé : 'graph_{MONTH}_{YEAR}.png'")

