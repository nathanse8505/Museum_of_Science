import pandas as pd
import re
import matplotlib.pyplot as plt
import numpy as np
from datetime import date
import os

# === LECTURE ET PARSING DU FICHIER ===
file_path = "LOG.TXT"
with open(file_path, "r", encoding="utf-8") as f:
    lines = f.readlines()

time_ms_list = []
time_hms_list = []
button_list = []
uv_status_list = []

offset_ms = 20 * 60 * 60 * 1000  # décalage à 20h

pattern = re.compile(r"(\d+)\s*ms\s*;\s*(SW\d)", re.IGNORECASE)
uv_keywords = ["led UV has activate", "la led UV", "UV a été activée", "UV activée"]

for line in lines:
    match = pattern.search(line)
    if match:
        time_ms = int(match.group(1))
        time_ms_with_offset = time_ms + offset_ms
        button = match.group(2)
        time_hms = pd.to_datetime(time_ms_with_offset, unit='ms')
        uv_activated = any(keyword.lower() in line.lower() for keyword in uv_keywords)

        time_ms_list.append(time_ms)
        time_hms_list.append(time_hms)
        button_list.append(button)
        uv_status_list.append("YES" if uv_activated else "NO")

df = pd.DataFrame({
    "Time_ms": time_ms_list,
    "Time": time_hms_list,
    "Button": button_list,
    "UV_Activated": uv_status_list
})

# === STATISTIQUES PAR BOUTON ===
grouped = df.groupby("Button").agg(
    Total_Clics=("UV_Activated", "count"),
    UV_Activations=("UV_Activated", lambda x: (x == "YES").sum())
).reset_index()

sw_with_uv = grouped[grouped["UV_Activations"] > 0]

with pd.ExcelWriter("statistiques_SW.xlsx") as writer:
    grouped.to_excel(writer, sheet_name="Tous les SW", index=False)
    sw_with_uv.to_excel(writer, sheet_name="SW avec UV activé", index=False)

print("Fichier 'statistiques_SW.xlsx' généré avec les deux tableaux.")

# === TRAITEMENT GRAPHIQUE PAR JOUR ===
df["Date"] = df["Time"].dt.date

df_no_uv = df[df["UV_Activated"] == "NO"]
count_no_uv = df_no_uv.groupby(["Date", "Button"]).size().unstack(fill_value=0)

df_yes_uv = df[df["UV_Activated"] == "YES"]
count_yes_uv = df_yes_uv.groupby(["Date", "Button"]).size().unstack(fill_value=0)

# Dates présentes dans les données
all_dates = sorted(set(df["Date"]))
x = np.arange(len(all_dates))

all_buttons = sorted(set(count_no_uv.columns).union(count_yes_uv.columns))
bar_width = 0.35

fig, ax = plt.subplots(figsize=(16, 8))

for i, button in enumerate(all_buttons):
    y_no_uv = [count_no_uv.at[d, button] if d in count_no_uv.index and button in count_no_uv.columns else 0 for d in all_dates]
    y_yes_uv = [count_yes_uv.at[d, button] if d in count_yes_uv.index and button in count_yes_uv.columns else 0 for d in all_dates]

    offset = i * (bar_width * 2 + 0.1)
    ax.bar(x + offset, y_no_uv, width=bar_width, color='orange', alpha=0.6)
    ax.bar(x + offset + bar_width, y_yes_uv, width=bar_width, color='blue', alpha=0.9)

# Axe X & légende
total_width = (bar_width * 2 + 0.1) * len(all_buttons)
ax.set_xticks(x + total_width / 2)
ax.set_xticklabels([d.strftime('%d/%m') for d in all_dates], rotation=45)

legend_patches = [
    plt.Line2D([0], [0], color='orange', lw=8, label='Sans UV'),
    plt.Line2D([0], [0], color='blue', lw=8, label='Avec UV')
]
ax.legend(handles=legend_patches, loc="upper left", bbox_to_anchor=(1.01, 1))
ax.set_title("Nombre d'appuis avec et sans UV par bouton – Toutes les dates")
ax.set_xlabel("Date")
ax.set_ylabel("Nombre d'appuis")
plt.tight_layout()
plt.savefig("comparaison_barres_par_bouton.png")
plt.show()

# === EXPORT DES DONNÉES JOURNALIÈRES ===
with pd.ExcelWriter("appuis_par_jour_par_bouton.xlsx") as writer:
    count_no_uv.to_excel(writer, sheet_name="Sans UV")
    count_yes_uv.to_excel(writer, sheet_name="Avec UV")
# === NOUVEL AFFICHAGE : division du temps en "jours virtuels" ===

# Paramètre : combien de jours virtuels tu veux ?
nombre_jours = 30

# Calcul du temps total
min_time = df["Time_ms"].min()
max_time = df["Time_ms"].max()
total_ms = max_time - min_time
ms_par_jour = total_ms / nombre_jours

# Créer une colonne "Jour_Virtuel" entre 0 et 29
df["Jour_Virtuel"] = ((df["Time_ms"] - min_time) / ms_par_jour).astype(int)

# Séparer UV / Sans UV
df_no_uv = df[df["UV_Activated"] == "NO"]
df_yes_uv = df[df["UV_Activated"] == "YES"]

# Groupement par jour
count_no_uv = df_no_uv.groupby(["Jour_Virtuel", "Button"]).size().unstack(fill_value=0)
count_yes_uv = df_yes_uv.groupby(["Jour_Virtuel", "Button"]).size().unstack(fill_value=0)

# Liste des jours et des boutons
jours = range(nombre_jours)
x = np.arange(nombre_jours)
all_buttons = sorted(set(count_no_uv.columns).union(count_yes_uv.columns))
bar_width = 0.35

# === GRAPHIQUE JOUR VIRTUEL ===
fig, ax = plt.subplots(figsize=(18, 8))

for i, button in enumerate(all_buttons):
    y_no_uv = [count_no_uv.at[j, button] if j in count_no_uv.index and button in count_no_uv.columns else 0 for j in jours]
    y_yes_uv = [count_yes_uv.at[j, button] if j in count_yes_uv.index and button in count_yes_uv.columns else 0 for j in jours]

    offset = i * (bar_width * 2 + 0.1)
    ax.bar(x + offset, y_no_uv, width=bar_width, color='orange', alpha=0.6)
    ax.bar(x + offset + bar_width, y_yes_uv, width=bar_width, color='blue', alpha=0.9)

# Axe X
total_width = (bar_width * 2 + 0.1) * len(all_buttons)
ax.set_xticks(x + total_width / 2)
ax.set_xticklabels([f"Jour {j+1}" for j in jours], rotation=45)

# Légende et titres
legend_patches = [
    plt.Line2D([0], [0], color='orange', lw=8, label='Sans UV'),
    plt.Line2D([0], [0], color='blue', lw=8, label='Avec UV')
]
ax.legend(handles=legend_patches, loc="upper left", bbox_to_anchor=(1.01, 1))
ax.set_title("Appuis avec et sans UV par jour virtuel (divisé selon le temps en ms)")
ax.set_xlabel("Jour virtuel")
ax.set_ylabel("Nombre d'appuis")
plt.tight_layout()
plt.savefig("barres_par_jour_virtuel.png")
plt.show()
