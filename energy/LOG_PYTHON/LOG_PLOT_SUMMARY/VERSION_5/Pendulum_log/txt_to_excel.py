
import pandas as pd
import re
from datetime import datetime
import matplotlib.pyplot as plt

# === LECTURE DU FICHIER LOG ===
log_file_path = "LOG.TXT"
with open(log_file_path, "r", encoding="utf-8") as f:
    lines = f.readlines()

# === INITIALISATION DES COMPTAGES ===
time_list = []
marker_list = []

# Pattern pour les deux types de messages recherchés
pattern_motor = re.compile(r"(\d+)\s*ms\s*;\s*Button pressed - Motor activated", re.IGNORECASE)
pattern_no_action = re.compile(r"(\d+)\s*ms\s*;\s*Button pressed \(no action triggered\)", re.IGNORECASE)

for line in lines:
    if pattern_motor.search(line):
        time_ms = int(pattern_motor.search(line).group(1))
        time_list.append(time_ms)
        marker_list.append("Motor activé")
    elif pattern_no_action.search(line):
        time_ms = int(pattern_no_action.search(line).group(1))
        time_list.append(time_ms)
        marker_list.append("No action")

# Décalage horaire pour conversion (20h en millisecondes)
offset_ms = 20 * 60 * 60 * 1000
time_with_offset = [t + offset_ms for t in time_list]

# Création du DataFrame
df = pd.DataFrame({
    "Time_ms": time_with_offset,
    "Marker": marker_list
})

# Tri des temps et conversion en dates réparties sur avril
df = df.sort_values(by="Time_ms").reset_index(drop=True)

# Répartition sur les 30 jours d'avril
total_entries = len(df)
average_per_day = total_entries // 30
remainder = total_entries % 30
dates_avril = [datetime(2025, 4, day).date() for day in range(1, 31)]

date_labels = []
for i in range(30):
    count = average_per_day + (1 if i < remainder else 0)
    date_labels.extend([dates_avril[i]] * count)

df["Date_Calculée"] = date_labels

# Regrouper et compter les types de markers par jour
grouped = df.groupby(["Date_Calculée", "Marker"]).size().unstack(fill_value=0)
grouped = grouped.rename(columns={"Motor activé": "Motor activé", "No action": "No action"})

# Calcul des totaux globaux
total_motor = grouped["Motor activé"].sum() if "Motor activé" in grouped.columns else 0
total_no_action = grouped["No action"].sum() if "No action" in grouped.columns else 0
total_all = total_motor + total_no_action

# Création du DataFrame final avec 6 colonnes
df_final = pd.DataFrame({
    "Date": grouped.index,
    "Motor activé": grouped.get("Motor activé", 0),
    "No action": grouped.get("No action", 0),
    "Total motor activé": [total_motor] + [None] * (len(grouped) - 1),
    "Total no action": [total_no_action] + [None] * (len(grouped) - 1),
    "Total": [total_all] + [None] * (len(grouped) - 1)
})

# Export Excel
df_final.to_excel("log.xlsx", index=False)

# Affichage graphique
plt.figure(figsize=(14, 6))
df_final.set_index("Date")[["Motor activé", "No action"]].plot(kind="bar", stacked=False)
plt.title(f"Number of press per month - Total: {total_all}")
plt.xlabel("Date")
plt.ylabel("Nombre")
plt.xticks(rotation=45, ha='right')
plt.legend()
plt.tight_layout()
plt.grid(axis='y')
plt.savefig("graphe_appuis_bouton.png")
#plt.show()
