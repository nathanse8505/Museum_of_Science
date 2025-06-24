import re
import matplotlib.pyplot as plt
import os
import pandas as pd

# === PARAMÈTRES ===
FILE_PATH = "LOG.TXT"
pattern_button = re.compile(r"(\d+)\s*ms\s*;\s*Button pressed", re.IGNORECASE)
pattern_init = re.compile(r"(\d+)\s*ms\s*;\s*Init", re.IGNORECASE)
motor_keywords = ["Motor activated"]
DELTA_TIME= 3600*1000*5
DOWNLOAD_DIR = os.path.expanduser("~/Downloads")


def plot_resume(df_resume, start_dt, end_dt):
    output_path = os.path.join(DOWNLOAD_DIR, "plot_resume.png")
    df_resume["Date"] = pd.to_datetime(df_resume["Date"])
    df_filtered = df_resume[(df_resume["Date"] >= start_dt) & (df_resume["Date"] <= end_dt)].copy()

    if df_filtered.empty:
        print("⚠️ Aucun jour dans l'intervalle spécifié pour le résumé.")
        return

    df_filtered.loc[:, "DateStr"] = df_filtered["Date"].dt.strftime("%Y-%m-%d")
    df_filtered = df_filtered.set_index("DateStr")

    plt.figure(figsize=(14, 6))
    df_filtered["MOTOR ON"].plot(kind="bar", color="orange", width=0.8)
    plt.title("Number of pushes per day  (motor activate)")
    plt.xlabel("Date")
    plt.ylabel("Number of pushes with motor ON")
    plt.xticks(rotation=45, ha='right')
    plt.tight_layout()
    plt.grid(axis='y')
    plt.savefig(output_path)
    print(f"📊 Graph saved: {output_path}")

