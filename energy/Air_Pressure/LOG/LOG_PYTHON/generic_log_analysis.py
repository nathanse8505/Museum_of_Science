import os
import sys
import re
import numpy as np
import matplotlib.pyplot as plt
from collections import defaultdict
from datetime import datetime


def sanitize_for_filename(s):
    return re.sub(r"[^\w\-]", "_", s)


def get_time_key(timestamp, interval):
    return timestamp.strftime("%Y-%m-%d %H:00") if interval == "hour" else timestamp.strftime("%Y-%m-%d")


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


def write_summary(result, interval, start_dt, end_dt):
    counters = result["counters"]
    first_dt = result["First Timestamp"]
    last_dt = result["Last Timestamp"]
    project_name = result["project_name"]

    def total(counter): return sum(counter.values())

    start_fmt = sanitize_for_filename(start_dt.strftime("%Y-%m-%d_%H-%M-%S"))
    end_fmt = sanitize_for_filename(end_dt.strftime("%Y-%m-%d_%H-%M-%S"))
    filename = f"{project_name}_summary_{start_fmt}_{end_fmt}.txt"

    total_intervals = len(set().union(*[d.keys() for d in counters.values()]))

    with open(filename, "w") as f:
        f.write(f"Summary from {start_dt} to {end_dt} ({(end_dt - start_dt).days + 1} days)\n")
        f.write(f"Log data time range: {first_dt} to {last_dt} ({(last_dt - first_dt).days + 1} days)\n\n")

        for label, counter in counters.items():
            total_count = total(counter)
            avg = total_count / total_intervals if total_intervals else 0
            f.write(f"Total {label}: {total_count}\n")
            f.write(f"Average {label} per {interval}: {avg:.2f}\n\n")

        overall_total = sum(total(c) for c in counters.values())
        f.write(f"Total event count: {overall_total}\n")

    print(f"Saved summary to: {filename}")


def plot_and_save_counts(result, interval, start_dt, end_dt):
    counters = result["counters"]
    project_name = result["project_name"]

    all_keys = sorted(set().union(*[counter.keys() for counter in counters.values()]))
    x = np.arange(len(all_keys))

    bar_width = 0.2
    offset = 0
    plt.figure(figsize=(12, 6))

    for label, counter in counters.items():
        counts = [counter.get(k, 0) for k in all_keys]
        plt.bar(x + offset, counts, width=bar_width, label=label)
        offset += bar_width

    plt.xticks(x + bar_width * len(counters) / 2.5, all_keys, rotation=45, ha="right")
    plt.ylabel("Count")
    plt.xlabel("Hour" if interval == "hour" else "Date")
    plt.title("Log Event Counts")
    plt.legend()
    plt.tight_layout()
    plt.grid(True, axis='y')

    start_fmt = sanitize_for_filename(start_dt.strftime("%Y-%m-%d_%H-%M-%S"))
    end_fmt = sanitize_for_filename(end_dt.strftime("%Y-%m-%d_%H-%M-%S"))
    filename = f"{project_name}_plot_{start_fmt}_{end_fmt}.png"
    plt.savefig(filename)
    print(f"Saved bar graph to: {filename}")
