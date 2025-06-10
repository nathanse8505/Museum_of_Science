import pandas as pd
import re
from datetime import datetime
import matplotlib.pyplot as plt

# === PARAMÈTRES ===
file_path = "graph/april/LOG_april.TXT"
offset_ms = 20 * 60 * 60 * 1000  # Décalage de 20h
pattern_button = re.compile(r"(\d+)\s*ms\s*;\s*(SW\d)", re.IGNORECASE)
uv_keywords = ["led UV has activate", "la led UV", "UV a été activée", "UV activée"]
# === LECTURE ET PARSING ===
current_day_index = 0
time_ms_list = []
button_list = []
uv_status_list = []
jour_logique_list = []
DAY = 10
MONTH = 5
YEAR = 2025

with open(file_path, "r", encoding="utf-8") as f:
    for line in f:
        if "init" in line:
            current_day_index += 1

        match = pattern_button.search(line)
        if match:
            raw_time = int(match.group(1))
            activated = any(keyword.lower() in line.lower() for keyword in uv_keywords)
            button = match.group(2)

            time_ms_list.append(raw_time)
            button_list.append(button)
            uv_status_list.append("YES" if activated else "NO")
            jour_logique_list.append(current_day_index)

# === ERREUR SI PAS D'INIT
if current_day_index == 0:
    raise ValueError("Aucun 'Init' trouvé : impossible d'assigner les jours.")

# === DATAFRAME PRINCIPAL ===
df = pd.DataFrame({
    "Time_ms": time_ms_list,
    "Button": button_list,
    "UV_Activated": uv_status_list,
    "Day": jour_logique_list
}).sort_values("Time_ms").reset_index(drop=True)
# === CONVERSION EN DATES
df["Date"] = df["Day"].apply(
    lambda j: datetime(YEAR, MONTH, DAY) + pd.Timedelta(days=j - 1)
)
# Réorganiser les colonnes dans df
cols = df.columns.tolist()
cols.remove("Date")
cols.remove("Day")
cols.insert(1, "Date")  # Position 1 = deuxième colonne (0-indexé)
df = df[cols]



# === RÉSUMÉ JOURNALIER ===
grouped = df.groupby(["Date", "UV_Activated"]).size().unstack(fill_value=0)
grouped = grouped.rename(columns={"NO": "Sans UV", "YES": "Avec UV"})

total_on = grouped.get("Avec UV", pd.Series()).sum()
total_off = grouped.get("Sans UV", pd.Series()).sum()
total_all = total_on + total_off

df_resume = pd.DataFrame({
    "Date": grouped.index,
    "Avec UV": grouped.get("Avec UV", 0),
    "Sans UV": grouped.get("Sans UV", 0),
    "Total avec UV": [total_on] + [None] * (len(grouped) - 1),
    "Total sans UV": [total_off] + [None] * (len(grouped) - 1),
    "Total": [total_all] + [None] * (len(grouped) - 1)
})


df_resume["Date"] = df_resume["Date"].astype(str)
df["Date"] = df["Date"].astype(str)


# === EXPORT EXCEL À 2 ONGLET ===
with pd.ExcelWriter("log_table.xlsx") as writer:
    df.to_excel(writer, sheet_name="Raw Data", index=False)
    df_resume.to_excel(writer, sheet_name="Résumé per day", index=False)

print("✅ Excel avec 2 onglets créé : 'log_table.xlsx'")

# === GRAPHIQUE ===
plt.figure(figsize=(14, 6))
df_resume.set_index("Date")["Avec UV"].plot(kind="bar", color="orange", width=0.8)
plt.title("Number of pushes per day (UV ACTIVATED)")
plt.xlabel(f"Date ({MONTH}/{YEAR})")
plt.ylabel("UV ON")
plt.xticks(rotation=45, ha='right')
plt.tight_layout()
plt.grid(axis='y')
plt.savefig("push_per_day.png")
# plt.show()

print("📊 Graphique sauvegardé : 'appuis_uv_par_jour.png'")

# === GRAPHIQUE PAR SWITCH ===
plt.figure(figsize=(14, 6))

# Filtrer uniquement les lignes où UV est activé
df_uv_on = df[df["UV_Activated"] == "YES"]

# Grouper par date et bouton
uv_switch_grouped = df_uv_on.groupby(["Date", "Button"]).size().unstack(fill_value=0)

# Tracer les courbes empilées par bouton
uv_switch_grouped.plot(kind="bar", stacked=False, figsize=(14, 6), width=0.8)

plt.title("Nombre d'appuis AVEC UV par bouton et par jour")
plt.xlabel(f"Date ({MONTH}/{YEAR})")
plt.ylabel("Nombre d'appuis AVEC UV")
plt.xticks(rotation=45, ha='right')
plt.legend(title="Bouton")
plt.tight_layout()
plt.grid(axis='y')
plt.savefig("appuis_uv_par_bouton.png")
# plt.show()

print("📊 Graphique par bouton sauvegardé : 'appuis_uv_par_bouton.png'")
