import os
import sys
import re
import numpy as np
import matplotlib.pyplot as plt
from collections import defaultdict
from datetime import datetime
import argparse

# Dossier de sauvegarde par défaut : Téléchargements de l'utilisateur
DOWNLOAD_DIR = os.path.expanduser("~/Downloads")

def parse_args():
    parser = argparse.ArgumentParser(description="Analyze logs in a datetime range.")
    parser.add_argument("files", nargs='+', help="Path(s) to text log file(s).")
    parser.add_argument("--start", required=True, help="Start datetime (\"YYYY-MM-DD HH:MM:SS\").")
    parser.add_argument("--end", required=True, help="End datetime (\"YYYY-MM-DD HH:MM:SS\").")
    parser.add_argument("--interval", choices=["day", "hour"], default="day",
                        help="Graph granularity: 'day' or 'hour'. Default is 'day'.")
    return parser.parse_args()

def sanitize_for_filename(s):
    return re.sub(r"[^\w\-]", "_", s)

def get_time_key(timestamp, interval):
    return timestamp.strftime("%Y-%m-%d %H:00") if interval == "hour" else timestamp.strftime("%Y-%m-%d")

def detect_project_name(file_path):
    patterns = {
        "Rocket Hydrogen": "The rocket has ignited",
        "Horsepower": "your horsepower is",
        "Jumping Ring": "Ring jumped!",
        "AirPressure": "The Bottle flew!"
    }
    try:
        with open(file_path, encoding="utf-8") as f:
            lines = f.read()
            for name, keyword in patterns.items():
                if keyword.lower() in lines.lower():
                    return name
    except Exception:
        pass
    return "project"  # fallback

def label_title(project):
    return {
        "Rocket Hydrogen": "The rocket has ignited",
        "Horsepower": "your horsepower is",
        "Jumping Ring": "Ring jumped!",
        "AirPressure": "The Bottle flew!"
    }.get(project, project.lower())

def analyze_logs(files, start_dt, end_dt, interval, event_config, project_name):
    counters = {label: defaultdict(int) for label in event_config}
    language_eng_counter = defaultdict(int)
    language_heb_counter = defaultdict(int)
    language_arb_counter = defaultdict(int)
    any_data_found = False
    first_dt = None
    last_dt = None

    for file_path in files:
        print(f"\nReading file: {file_path}")
        if not os.path.isfile(file_path):
            print(f"File not found: {file_path}", file=sys.stderr)
            continue

        with open(file_path, "r", encoding="utf-8") as f:
            for line in f:
                try:
                    timestamp_str = line.split(" - ")[0]
                    timestamp = datetime.strptime(timestamp_str, "%Y-%m-%d %H:%M:%S")

                    if not (start_dt <= timestamp <= end_dt):
                        continue

                    if first_dt is None or timestamp < first_dt:
                        first_dt = timestamp
                    if last_dt is None or timestamp > last_dt:
                        last_dt = timestamp

                    time_key = get_time_key(timestamp, interval)
                    line_lower = line.lower()

                    for label, keyword in event_config.items():
                        if keyword.lower() in line_lower:
                            counters[label][time_key] += 1
                            any_data_found = True

                    if "your language is: english" in line_lower:
                        language_eng_counter[time_key] += 1
                        any_data_found = True
                    if "your language is: hebrew" in line_lower:
                        language_heb_counter[time_key] += 1
                        any_data_found = True
                    if "your language is: arabic" in line_lower:
                        language_arb_counter[time_key] += 1
                        any_data_found = True

                except Exception:
                    print(f"Skipping line (parse error): {line.rstrip()}", file=sys.stderr)

    if not any_data_found:
        return None

    counters["Language: English"] = language_eng_counter
    counters["Language: Hebrew"] = language_heb_counter
    counters["Language: Arabic"] = language_arb_counter

    return {
        "counters": counters,
        "First Timestamp": first_dt,
        "Last Timestamp": last_dt,
        "project_name": project_name
    }

def write_summary(data_dict, interval, start_dt, end_dt):
    def total(counter):
        return sum(counter.values())

    first_dt = data_dict["First Timestamp"]
    last_dt = data_dict["Last Timestamp"]
    counters = data_dict["counters"]
    project_name = data_dict["project_name"]
    label_text = label_title(project_name)

    filename = os.path.join(DOWNLOAD_DIR, f"{project_name}_summary.txt")

    total_intervals = len(set().union(*[d.keys() for d in counters.values()]))

    ring_total = total(counters.get(project_name, defaultdict(int)))
    eng_total = total(counters.get("Language: English", defaultdict(int)))
    heb_total = total(counters.get("Language: Hebrew", defaultdict(int)))
    arb_total = total(counters.get("Language: Arabic", defaultdict(int)))
    total_langs = eng_total + heb_total + arb_total

    ring_avg = ring_total / total_intervals if total_intervals else 0
    eng_avg = eng_total / total_intervals if total_intervals else 0
    heb_avg = heb_total / total_intervals if total_intervals else 0
    arb_avg = arb_total / total_intervals if total_intervals else 0
    total_langs_avg = total_langs / total_intervals if total_intervals else 0

    denom = ring_total + total_langs
    good_runs_pct = (ring_total / denom) * 100 if denom else 0

    with open(filename, "w") as f:
        f.write(f"Summary from {start_dt} to {end_dt} ({(end_dt - start_dt).days + 1} days)\n")
        f.write(f"Log data time range: {first_dt} to {last_dt} ({(last_dt - first_dt).days + 1} days)\n\n")

        f.write(f"Total {label_text}: {ring_total}\n")
        f.write(f"Total english language: {eng_total}\n")
        f.write(f"Total hebrew language: {heb_total}\n")
        f.write(f"Total arabic language: {arb_total}\n")
        f.write(f"Total language changes: {total_langs}\n\n")

        f.write(f"Average {label_text} per {interval}: {ring_avg:.2f}\n")
        f.write(f"Average english language per {interval}: {eng_avg:.2f}\n")
        f.write(f"Average hebrew language per {interval}: {heb_avg:.2f}\n")
        f.write(f"Average arabic language per {interval}: {arb_avg:.2f}\n")
        f.write(f"Average total language changes per {interval}: {total_langs_avg:.2f}\n\n")

        f.write(f"Good runs: {ring_total} / {denom} ({good_runs_pct:.2f}%)\n")

    print(f"\u2705 Saved summary to: {filename}")

def plot_and_save_counts(data_dict, interval):
    counters = data_dict["counters"]
    project_name = data_dict["project_name"]

    all_keys = sorted(set().union(*[counter.keys() for counter in counters.values()]))
    x = np.arange(len(all_keys))

    bar_width = 0.2
    offset = 0
    plt.figure(figsize=(12, 6))

    lang_labels = ["Language: English", "Language: Hebrew", "Language: Arabic"]

    for label in [project_name] + lang_labels:
        if label in counters:
            counts = [counters[label].get(k, 0) for k in all_keys]
            plt.bar(x + offset, counts, width=bar_width, label=label)
            offset += bar_width

    plt.xticks(x + bar_width * len(counters) / 2.5, all_keys, rotation=45, ha="right")
    plt.ylabel("Count")
    plt.xlabel("Hour" if interval == "hour" else "Date")
    plt.title("Log Event Counts")
    plt.legend()
    plt.tight_layout()
    plt.grid(True, axis='y')

    filename = os.path.join(DOWNLOAD_DIR, f"{project_name}_plot.png")
    plt.savefig(filename)
    print(f"\u2705 Saved bar graph to: {filename}")
