from CONST_n_PLOT import *


def build_df(time_ms_list, motor_status_list, jour_logique_list):
    # === DATAFRAME PRINCIPAL ===
    df = pd.DataFrame({
        "Time_ms": time_ms_list,
        "Motor_Activated": motor_status_list,
        "Day": jour_logique_list
    })
    return df


def timestamp_to_date(df, start_dt):
    # === CONVERSION EN DATES
    df["Date"] = df["Day"].apply(
        lambda j: start_dt + pd.Timedelta(days=j)
    )
    return df


def organize_df(df):
    # Réorganiser les colonnes dans df
    cols = df.columns.tolist()
    cols.remove("Date")
    cols.remove("Day")
    cols.insert(1, "Date")  # Position 1 = deuxième colonne (0-indexé)
    df = df[cols]
    return df


def build_df_resume(df):
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

    return df_resume


def export_excel(df, df_resume):
    df_resume["Date"] = df_resume["Date"].astype(str)
    df["Date"] = df["Date"].astype(str)
    output_path = os.path.join(DOWNLOAD_DIR, f"log_table.xlsx")
    # === EXPORT EXCEL À 2 ONGLET ===
    with pd.ExcelWriter(output_path) as writer:
        df.to_excel(writer, sheet_name="Raw Data", index=False)
        df_resume.to_excel(writer, sheet_name="Résumé per day", index=False)

    print("✅ Excel avec 2 onglets créé : 'log_table.xlsx'")


def write_summary_pendulum(df, start_dt, end_dt):
    # Convertir la colonne Date
    project_name = "Pendulum"
    interval = "day"

    # Convertir et tronquer la colonne Date
    df["Date"] = pd.to_datetime(df["Date"])

    # Appliquer un filtre strict entre start_dt et end_dt
    df_filtered = df[(df["Date"] <= end_dt)]

    if df_filtered.empty:
        print("⚠️ Aucun événement dans l’intervalle de temps spécifié.")
        return

    output_path = os.path.join(DOWNLOAD_DIR, f"{project_name}_summary.txt")

    first_dt = df_filtered["Date"].min()
    last_dt = df_filtered["Date"].max()

    df_range = df_filtered[(df_filtered["Date"] >= first_dt) & (df_filtered["Date"] <= last_dt)]

    uv_activated_total = (df_range["Motor_Activated"] == "YES").sum()
    sw_off_total = (df_range["Motor_Activated"] == "NO").sum()
    total_events = len(df_range)

    nb_periods = df_range["Date"].nunique() or 1

    uv_avg = uv_activated_total / nb_periods
    sw_off_avg = sw_off_total / nb_periods
    total_avg = total_events / nb_periods

    with open(output_path, "w", encoding="utf-8") as f:
        f.write(f"Summary from {start_dt.strftime('%Y-%m-%d')} to {end_dt.strftime('%Y-%m-%d')} "
                f"({(end_dt - start_dt).days + 1} days)\n")
        f.write(f"Log data time range: {first_dt.strftime('%Y-%m-%d')} to {last_dt.strftime('%Y-%m-%d')} "
                f"({(last_dt - first_dt).days + 1} days)\n\n")

        f.write(f"Total MOTOR activated: {uv_activated_total}\n")
        f.write(f"Total MOTOR NO ACTION: {sw_off_total}\n")
        f.write(f"Total actions: {total_events}\n\n")

        f.write(f"Average MOTOR activated per {interval}: {uv_avg:.2f}\n")
        f.write(f"Average  MOTOR NO ACTION per {interval}: {sw_off_avg:.2f}\n")
        f.write(f"Average total actions per {interval}: {total_avg:.2f}\n")

    print(f"📄 Résumé sauvegardé dans {output_path}")
