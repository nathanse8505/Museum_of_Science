import re
import matplotlib.pyplot as plt
import os
import pandas as pd

# === PARAMÈTRES ===
FILE_PATH = "graph_and_data/april/LOG_april.TXT"
# file_path = "graph/april/simulated_log.TXT"
pattern_button = re.compile(r"(\d+)\s*ms\s*;\s*(SW\d)", re.IGNORECASE)
pattern_init = re.compile(r"(\d+)\s*ms\s*;\s*init", re.IGNORECASE)
uv_keywords = ["led UV has activate", "la led UV", "UV a été activée", "UV activée"]
DELTA_TIME = 3600 * 1000 * 5  # if there are 5 hour between init you can count a day
DOWNLOAD_DIR = os.path.expanduser("~/Downloads")


def plot_resume(df_resume, start_dt, end_dt):
    # Convertir les dates si besoin
    output_path = os.path.join(DOWNLOAD_DIR, f" plot_resume.png")
    df_resume["Date"] = pd.to_datetime(df_resume["Date"])
    df_filtered = df_resume[(df_resume["Date"] >= start_dt) & (df_resume["Date"] <= end_dt)]

    if df_filtered.empty:
        print("⚠️ Aucun jour dans l'intervalle spécifié pour le résumé.")
        return

    plt.figure(figsize=(14, 6))
    df_filtered.set_index("Date")["Avec UV"].plot(kind="bar", color="orange", width=0.8)
    plt.title("Number of pushes per day (UV ACTIVATED)")
    plt.xlabel(f"Date ({start_dt.month}/{start_dt.year})")
    plt.ylabel("UV ON")
    plt.xticks(rotation=45, ha='right')
    plt.tight_layout()
    plt.grid(axis='y')
    plt.savefig(output_path)
    print("📊 Graphique sauvegardé : 'push_per_day.png'")


def plot_resume_per_SW(df, start_dt, end_dt):
    # Convertir les dates si besoin
    output_path = os.path.join(DOWNLOAD_DIR, f" plot_resume_per_SW.png")
    df["Date"] = pd.to_datetime(df["Date"])
    df_uv_on = df[(df["UV_Activated"] == "YES") & (df["Date"] >= start_dt) & (df["Date"] <= end_dt)]

    if df_uv_on.empty:
        print("⚠️ Aucun appui UV activé dans l'intervalle donné.")
        return

    uv_switch_grouped = df_uv_on.groupby(["Date", "Button"]).size().unstack(fill_value=0)

    plt.figure(figsize=(14, 6))
    uv_switch_grouped.plot(kind="bar", stacked=False, figsize=(14, 6), width=0.8)
    plt.title("Nombre d'appuis AVEC UV par bouton et par jour")
    plt.xlabel(f"Date ({start_dt.month}/{start_dt.year})")
    plt.ylabel("Nombre d'appuis AVEC UV")
    plt.xticks(rotation=45, ha='right')
    plt.legend(title="Bouton")
    plt.tight_layout()
    plt.grid(axis='y')
    plt.savefig(output_path)
    print("📊 Graphique par bouton sauvegardé : 'appuis_uv_par_bouton.png'")
