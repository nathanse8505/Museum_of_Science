import pandas as pd
import re
from datetime import datetime, timedelta
import matplotlib.pyplot as plt

# === LECTURE DU FICHIER TEXTE ===
file_path = r"graph/april/LOG_april.TXT"
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

        time_ms_list.append(time_ms)
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

# === AFFICHAGE EN OPTION ===
print(df.head())
# === EXPORT VERS EXCEL ===
# === CALCUL DE LA MOYENNE JOURNALIÈRE D'ACTIVATION UV ===
total_uv_activations = df[df["UV_Activated"] == "YES"].shape[0]
moyenne_uv_par_jour = total_uv_activations / 30

print(f"\nTotal d'activations UV : {total_uv_activations}")
print(f"Moyenne d'activations UV par jour : {moyenne_uv_par_jour:.2f}")

output_file = "appuis_par_jour.xlsx"
df.to_excel(output_file, index=False)
print(f"\nFichier '{output_file}' généré avec succès.")


##################plot##################



# Créer un DataFrame groupé par date et UV_Activated pour compter les pressions
grouped_counts = df.groupby(["Date_Calculée", "UV_Activated"]).size().unstack(fill_value=0)

# S'assurer que les colonnes sont bien nommées
grouped_counts = grouped_counts.rename(columns={"NO": "Appuis_Sans_UV", "YES": "Appuis_Avec_UV"})

# Tracer uniquement les appuis AVEC activation UV
plt.figure(figsize=(14, 6))
grouped_counts["Appuis_Avec_UV"].plot(kind='bar', color='orange', width=0.8)

plt.title("Nombre d'appuis avec activation UV par jour")
plt.xlabel("Date (avril 2025)")
plt.ylabel("Nombre d'appuis avec UV")
plt.xticks(rotation=45, ha='right')
plt.tight_layout()
plt.grid(axis='y')

plt.savefig("appuis_uv_par_jour.png")  # 📁 Image sauvegardée dans le dossier courant
# Afficher le graphique
#plt.show()




# # Tracer les barres côte à côte
# plt.figure(figsize=(14, 6))
# grouped_counts.plot(kind='bar', stacked=False, width=0.8)
#
# plt.title("Nombre d'appuis par jour - Avec et sans activation UV")
# plt.xlabel("Date (avril 2025)")
# plt.ylabel("Nombre d'appuis")
# plt.xticks(rotation=45, ha='right')
# plt.legend(title="Type d'appui")
# plt.tight_layout()
# plt.grid(axis='y')
#
# # Afficher le graphique
# plt.show()
