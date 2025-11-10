import pandas as pd
from datetime import datetime
from CONST_n_PLOT import YEAR,MONTH,DAY

def build_df(time_ms_list,button_list,uv_status_list,jour_logique_list):
    # === DATAFRAME PRINCIPAL ===
    df = pd.DataFrame({
        "Time_ms": time_ms_list,
        "Button": button_list,
        "UV_Activated": uv_status_list,
        "Day": jour_logique_list
    }).reset_index(drop=True)
    return df


def timestamp_to_date(df):
    # === CONVERSION EN DATES
    df["Date"] = df["Day"].apply(
        lambda j: datetime(YEAR, MONTH, DAY) + pd.Timedelta(days=j - 1)
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
    # === EXPORT EXCEL À 2 ONGLET ===
    with pd.ExcelWriter("log_table.xlsx") as writer:
        df.to_excel(writer, sheet_name="Raw Data", index=False)
        df_resume.to_excel(writer, sheet_name="Résumé per day", index=False)

    print("✅ Excel avec 2 onglets créé : 'log_table.xlsx'")