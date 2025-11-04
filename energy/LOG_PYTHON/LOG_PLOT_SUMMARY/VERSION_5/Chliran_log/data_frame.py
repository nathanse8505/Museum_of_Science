from CONST_n_PLOT import *

def build_df(time_ms_list,button_list,uv_status_list,jour_logique_list):
    # === DATAFRAME PRINCIPAL ===
    df = pd.DataFrame({
        "Time_ms": time_ms_list,
        "Button": button_list,
        "UV_Activated": uv_status_list,
        "Day": jour_logique_list
    }).reset_index(drop=True)
    return df


def timestamp_to_date(df,start_dt):
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
    return df_resume

def export_excel(df,df_resume):
    output_path = os.path.join(DOWNLOAD_DIR, f"log_table.xlsx")
    # === EXPORT EXCEL À 2 ONGLET ===
    with pd.ExcelWriter(output_path) as writer:
        df.to_excel(writer, sheet_name="Raw Data", index=False)
        df_resume.to_excel(writer, sheet_name="Résumé per day", index=False)

    print("✅ Excel avec 2 onglets créé : 'log_table.xlsx'")



def write_summary_chliran(df, start_dt, end_dt):
    # Convertir la colonne Date
    project_name = "Chliran"
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

    uv_activated_total = (df_range["UV_Activated"] == "YES").sum()
    sw_off_total = (df_range["UV_Activated"] == "NO").sum()
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

        f.write(f"UV activated: {uv_activated_total}\n")
        f.write(f"Total SW off: {sw_off_total}\n")
        f.write(f"Total actions: {total_events}\n\n")

        f.write(f"Average UV activated per {interval}: {uv_avg:.2f}\n")
        f.write(f"Average SW off per {interval}: {sw_off_avg:.2f}\n")
        f.write(f"Average total actions per {interval}: {total_avg:.2f}\n")

    print(f"📄 Résumé sauvegardé dans {output_path}")
