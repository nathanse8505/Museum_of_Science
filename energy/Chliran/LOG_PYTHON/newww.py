import pandas as pd
import re
from datetime import datetime
import matplotlib.pyplot as plt

# === PARAMÈTRES ===
file_path = "graph/april/LOG_april.TXT"
offset_ms = 20 * 60 * 60 * 1000  # Décalage de 20h
pattern = re.compile(r"(\d+)\s*ms\s*;\s*(SW\d)", re.IGNORECASE)
uv_keywords = ["led UV has activate", "la led UV", "UV a été activée", "UV activée"]

# === LECTURE ET PARSING ===
time_ms_raw = []
time_ms_offset = []
button_list = []
uv_status_list = []

with open(file_path, "r", encoding="utf-8") as f:
    lines = f.readlines()

for line in lines:
    match = pattern.search(line)
    if match:
        raw_time = int(match.group(1))
        time_ms = raw_time + offset_ms
        button = match.group(2)
        activated = any(keyword.lower() in line.lower() for keyword in uv_keywords)

        time_ms_raw.append(raw_time)
        time_ms_offset.append(time_ms)
        button_list.append(button)
        uv_status_list.append("YES" if activated else "NO")

# === DATAFRAME PRINCIPAL ===
df = pd.DataFrame({
    "Time_ms_raw": time_ms_raw,
    "Time_ms": time_ms_offset,  # avec offset
    "Button": button_list,
    "UV_Activated": uv_status_list
}).sort_values("Time_ms").reset_index(drop=True)

# === DISTRIBUTION PAR JOUR ===
total_pushes = len(df)
average_per_day = total_pushes // 30
remainder = total_pushes % 30

dates_avril = [datetime(2025, 4, day).date() for day in range(1, 31)]
date_labels = []
for i in range(30):
    count = average_per_day + (1 if i < remainder else 0)
    date_labels.extend([dates_avril[i]] * count)

df["Date_Calculée"] = date_labels

# === RÉSUMÉ JOURNALIER ===
grouped = df.groupby(["Date_Calculée", "UV_Activated"]).size().unstack(fill_value=0)
grouped = grouped.rename(columns={"NO": "Sans UV", "YES": "Avec UV"})

total_avec_uv = grouped.get("Avec UV", pd.Series()).sum()
total_sans_uv = grouped.get("Sans UV", pd.Series()).sum()
total_all = total_avec_uv + total_sans_uv

df_resume = pd.DataFrame({
    "Date": grouped.index,
    "Avec UV": grouped.get("Avec UV", 0),
    "Sans UV": grouped.get("Sans UV", 0),
    "Total avec UV": [total_avec_uv] + [None] * (len(grouped) - 1),
    "Total sans UV": [total_sans_uv] + [None] * (len(grouped) - 1),
    "Total": [total_all] + [None] * (len(grouped) - 1)
})

# === EXPORT EXCEL À 2 ONGLET ===
with pd.ExcelWriter("fusion_appuis_uv.xlsx") as writer:
    df.to_excel(writer, sheet_name="Raw Data", index=False)
    df_resume.to_excel(writer, sheet_name="Résumé par jour", index=False)

print("✅ Excel avec 2 onglets créé : 'fusion_appuis_uv.xlsx'")

# === GRAPHIQUE ===
plt.figure(figsize=(14, 6))
df_resume.set_index("Date")["Avec UV"].plot(kind="bar", color="orange", width=0.8)
plt.title("Nombre d'appuis avec activation UV par jour")
plt.xlabel("Date (avril 2025)")
plt.ylabel("Nombre d'appuis avec UV")
plt.xticks(rotation=45, ha='right')
plt.tight_layout()
plt.grid(axis='y')
plt.savefig("appuis_uv_par_jour.png")
# plt.show()

print("📊 Graphique sauvegardé : 'appuis_uv_par_jour.png'")

# === GRAPHIQUE PAR SWITCH ===
plt.figure(figsize=(14, 6))

# Filtrer uniquement les lignes où UV est activé
df_uv_on = df[df["UV_Activated"] == "YES"]

# Grouper par date et bouton
uv_switch_grouped = df_uv_on.groupby(["Date_Calculée", "Button"]).size().unstack(fill_value=0)

# Tracer les courbes empilées par bouton
uv_switch_grouped.plot(kind="bar", stacked=False, figsize=(14, 6), width=0.8)

plt.title("Nombre d'appuis AVEC UV par bouton et par jour")
plt.xlabel("Date (avril 2025)")
plt.ylabel("Nombre d'appuis AVEC UV")
plt.xticks(rotation=45, ha='right')
plt.legend(title="Bouton")
plt.tight_layout()
plt.grid(axis='y')
plt.savefig("appuis_uv_par_bouton.png")
# plt.show()

print("📊 Graphique par bouton sauvegardé : 'appuis_uv_par_bouton.png'")
