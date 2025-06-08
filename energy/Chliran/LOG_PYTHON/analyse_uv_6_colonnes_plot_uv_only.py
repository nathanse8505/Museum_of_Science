
import pandas as pd
import re
from datetime import datetime
import matplotlib.pyplot as plt

# === LECTURE DU FICHIER TEXTE ===
file_path = "LOG_april.TXT"
with open(file_path, "r", encoding="utf-8") as f:
    lines = f.readlines()

# === INITIALISATION ===
time_ms_list = []
button_list = []
uv_status_list = []

offset_ms = 20 * 60 * 60 * 1000  # Décalage de 20h
pattern = re.compile(r"(\d+)\s*ms\s*;\s*(SW\d)", re.IGNORECASE)
uv_keywords = ["led UV has activate", "la led UV", "UV a été activée", "UV activée"]

# === PARSING DU FICHIER ===
for line in lines:
    match = pattern.search(line)
    if match:
        time_ms = int(match.group(1))
        time_ms_with_offset = time_ms + offset_ms
        button = match.group(2)
        uv_activated = any(keyword.lower() in line.lower() for keyword in uv_keywords)

        time_ms_list.append(time_ms_with_offset)
        button_list.append(button)
        uv_status_list.append("YES" if uv_activated else "NO")

# === CONSTRUCTION DU DATAFRAME ===
df = pd.DataFrame({
    "Time_ms": time_ms_list,
    "Button": button_list,
    "UV_Activated": uv_status_list
})

# === TRI PAR TEMPS ===
df = df.sort_values(by="Time_ms").reset_index(drop=True)

# === RÉPARTITION DES DATES DU 1 AU 30 AVRIL SELON MOYENNE ===
total_pushes = len(df)
average_per_day = total_pushes // 30
remainder = total_pushes % 30

# Générer la liste des dates
dates_avril = [datetime(2025, 4, day).date() for day in range(1, 31)]

# Attribuer les dates en fonction du nombre d'appuis par jour
date_labels = []
for day_index in range(30):
    count = average_per_day + (1 if day_index < remainder else 0)
    date_labels.extend([dates_avril[day_index]] * count)

# Ajouter la date calculée
df["Date_Calculée"] = date_labels

# === REGROUPEMENT PAR JOUR ===
grouped_counts = df.groupby(["Date_Calculée", "UV_Activated"]).size().unstack(fill_value=0)
grouped_counts = grouped_counts.rename(columns={"NO": "Sans UV", "YES": "Avec UV"})

# Totaux globaux
total_avec_uv = grouped_counts["Avec UV"].sum() if "Avec UV" in grouped_counts.columns else 0
total_sans_uv = grouped_counts["Sans UV"].sum() if "Sans UV" in grouped_counts.columns else 0
total_global = total_avec_uv + total_sans_uv

# Création du DataFrame final avec 6 colonnes
df_final = pd.DataFrame({
    "Date": grouped_counts.index,
    "Avec UV": grouped_counts.get("Avec UV", 0),
    "Sans UV": grouped_counts.get("Sans UV", 0),
    "Total avec UV": [total_avec_uv] + [None] * (len(grouped_counts) - 1),
    "Total sans UV": [total_sans_uv] + [None] * (len(grouped_counts) - 1),
    "Total": [total_global] + [None] * (len(grouped_counts) - 1)
})

# === EXPORT VERS EXCEL ===
df_final.to_excel("résumé_appuis_uv_6_colonnes.xlsx", index=False)

# === AFFICHAGE DU GRAPHIQUE : seulement les appuis avec UV ===
plt.figure(figsize=(14, 6))
df_final.set_index("Date")["Avec UV"].plot(kind="bar", color="orange", label=f"Avec UV (Total: {total_avec_uv})")
plt.title(f"Nombre d'appuis AVEC UV par mois - Total: {total_avec_uv}")
plt.xlabel("Date")
plt.ylabel("Nombre d'appuis AVEC UV")
plt.xticks(rotation=45, ha='right')
plt.legend()
plt.tight_layout()
plt.grid(axis='y')
plt.savefig("graphique_appuis_uv_uniquement.png")
plt.show()
