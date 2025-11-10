from CONST_n_PLOT import *
from data_frame import *

# === LECTURE ET PARSING ===
current_day_index = 0
time_ms_list = []
button_list = []
uv_status_list = []
jour_logique_list = []
previous_init_reference_time = None
prev_raw_time = 0
consecutive_init = False
is_last_init = False

def main(file_path = FILE_PATH):
    with open(file_path, "r", encoding="utf-8") as f:
        all_lines = f.readlines()

    init_indices = [i for i, line in enumerate(all_lines) if pattern_init.search(line)]

    for idx, line in enumerate(all_lines):

        match_button = pattern_button.search(line)
        match_init = pattern_init.search(line)
        raw_time_match = re.search(r"(\d+)", line)
        raw_time = int(raw_time_match.group(1))  # trouve le temp sur chaque ligne

        if match_init:
            if consecutive_init or idx == init_indices[0]:
                previous_init_reference_time = raw_time
                continue  # ignorer les init consécutifs

            # Conserver le timestamp de la ligne précédente pour delta
            if not is_last_init:
                prev_line = all_lines[idx - 1]
                prev_match = re.search(r"(\d+)", prev_line)
                if prev_match:
                    prev_raw_time = int(prev_match.group(1))

            delta = prev_raw_time - previous_init_reference_time
            #print(delta, prev_raw_time, previous_init_reference_time)

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
            button = match_button.group(2)
            activated = any(keyword.lower() in line.lower() for keyword in uv_keywords)

            time_ms_list.append(raw_time)
            button_list.append(button)
            uv_status_list.append("YES" if activated else "NO")
            jour_logique_list.append(current_day_index)

    # === ERREUR SI PAS D'INIT
    if current_day_index == 0:
        raise ValueError("Aucun 'Init' trouvé : impossible d'assigner les jours.")

    df = build_df(time_ms_list, button_list, uv_status_list, jour_logique_list)
    df = timestamp_to_date(df)
    df = organize_df(df)

    df_resume = build_df_resume(df)

    df_resume["Date"] = df_resume["Date"].astype(str)
    df["Date"] = df["Date"].astype(str)
    # === EXPORT EXCEL À 2 ONGLET ===
    export_excel(df,df_resume)
    # === GRAPHIQUE RESUME===
    plot_resume(df_resume)
    # === GRAPHIQUE PAR SWITCH ===
    plot_resume_per_SW(df)

if __name__ == "main":
    main()