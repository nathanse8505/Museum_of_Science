import pandas as pd
import re
from datetime import datetime
import matplotlib.pyplot as plt


# === PARAMÈTRES ===

file_path = "LOG.TXT"
pattern_button = re.compile(r"(\d+)\s*ms\s*;\s*Button pressed", re.IGNORECASE)
pattern_init = re.compile(r"(\d+)\s*ms\s*;\s*Init", re.IGNORECASE)
motor_keywords = ["Motor activated"]
DELTA_TIME= 3600*1000*5
DAY = 1
MONTH = 5
YEAR = 2025
# === LECTURE ET PARSING ===
current_day_index = 0
time_ms_list = []
motor_status_list = []
jour_logique_list = []
previous_init_reference_time = None
prev_raw_time = 0
consecutive_init = False
is_last_init = False


with open(file_path, "r", encoding="utf-8") as f:
    all_lines = f.readlines()
    init_indices = [i for i, line in enumerate(all_lines) if pattern_init.search(line)]

for idx, line in enumerate(all_lines):
    match_button = pattern_button.search(line)
    match_init = pattern_init.search(line)
    raw_time_match = re.search(r"(\d+)", line)
    raw_time = int(raw_time_match.group(1)) #trouve le temp sur chaque ligne

    if match_init is not None:
        if consecutive_init or idx == init_indices[0]:
            previous_init_reference_time = raw_time
            #print(previous_init_reference_time)
            continue  # ignorer les init consécutifs

        # Conserver le timestamp de la ligne précédente pour delta
        if not is_last_init:
            prev_line = all_lines[idx - 1]
            prev_match = re.search(r"(\d+)", prev_line)
            if prev_match:
                prev_raw_time = int(prev_match.group(1))

        delta = prev_raw_time - previous_init_reference_time
        #print(delta,prev_raw_time,previous_init_reference_time)

        if delta > DELTA_TIME:  # 5 heures en ms
            current_day_index += 1

        previous_init_reference_time = raw_time
        consecutive_init = True

        is_last_init = idx == init_indices[-1]  # dernier init connu dans le fichier
        if is_last_init:
            current_day_index += 1
            print(f"Dernier init → jour +1 → index {current_day_index}")
    else:
        consecutive_init = False

    if match_button:
            raw_time = int(match_button.group(1))
            motor = any(keyword.lower() in line.lower() for keyword in motor_keywords)

            time_ms_list.append(raw_time)
            motor_status_list.append("YES" if motor else "NO")
            jour_logique_list.append(current_day_index)



# === ERREUR SI PAS D'INIT
if current_day_index == 0:
    raise ValueError("Aucun 'Init' trouvé : impossible d'assigner les jours.")

# === CONSTRUCTION DU DATAFRAME
df = pd.DataFrame({
    "Time_ms": time_ms_list,
    "Motor_Activated": motor_status_list,
    "Day": jour_logique_list
})

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
grouped = df.groupby(["Date", "Motor_Activated"]).size().unstack(fill_value=0)
grouped = grouped.rename(columns={"NO": "NO ACTION", "YES": "MOTOR ON"})

total_on = grouped.get("MOTOR ON", pd.Series()).sum()
total_off = grouped.get("NO ACTION", pd.Series()).sum()
total_all = total_on + total_off

df_resume = pd.DataFrame({
    "Date": grouped.index,
    "MOTOR ON": grouped.get("MOTOR ON", 0),
    "NO ACTION": grouped.get("NO ACTION", 0),
    "Total with Motor": [total_on] + [None] * (len(grouped) - 1),
    "Total NO ACTION": [total_off] + [None] * (len(grouped) - 1),
    "Total": [total_all] + [None] * (len(grouped) - 1)
})

df_resume["Date"] = df_resume["Date"].astype(str)
df["Date"] = df["Date"].astype(str)

# === EXPORT EXCEL À 2 ONGLET ===
with pd.ExcelWriter(f"df_{MONTH}.xlsx") as writer:
    df.to_excel(writer, sheet_name="Raw Data", index=False)
    df_resume.to_excel(writer, sheet_name="Résumé per day", index=False)

print(f"✅ Excel avec 2 onglets créé : df_{MONTH}.xlsx")

# === GRAPHIQUE ===
plt.figure(figsize=(14, 6))
df_resume.set_index("Date")["MOTOR ON"].plot(kind="bar", color="orange", width=0.8)
plt.title("Number of pushes per day (motor activate)")
plt.xlabel(f"Date ({MONTH}/{YEAR})")
plt.ylabel("Number of pushes with motor ON")
plt.xticks(rotation=45, ha='right')
plt.tight_layout()
plt.grid(axis='y')
plt.savefig(f"graph_{MONTH}_{YEAR}.png")
# plt.show()

print(f"📊 Graphique sauvegardé : 'graph_{MONTH}_{YEAR}.png'")

