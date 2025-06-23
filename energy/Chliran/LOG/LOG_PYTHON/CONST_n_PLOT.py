import re
import matplotlib.pyplot as plt
# === PARAMÈTRES ===
FILE_PATH = "graph_and_data/april/LOG_april.TXT"
#file_path = "graph/april/simulated_log.TXT"
pattern_button = re.compile(r"(\d+)\s*ms\s*;\s*(SW\d)", re.IGNORECASE)
pattern_init = re.compile(r"(\d+)\s*ms\s*;\s*init", re.IGNORECASE)
uv_keywords = ["led UV has activate", "la led UV", "UV a été activée", "UV activée"]
DELTA_TIME= 3600*1000*5 # if there are 5 hour between init you can count a day
DAY = 10
MONTH = 5
YEAR = 2025

def plot_resume(df_resume):
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

def plot_resume_per_SW(df):
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
