import argparse
from datetime import datetime
from collections import defaultdict
import matplotlib.pyplot as plt
import sys
import re
import os
import numpy as np

FILE_PATH = ""
INTERVAL = "day" # "day" or "hour"
START_DT = "YYYY-MM-DD HH:MM:SS"
END_DT = "YYYY-MM-DD HH:MM:SS"

def parse_args():
    parser = argparse.ArgumentParser(description="Analyze logs in a datetime range.")
    parser.add_argument("files", nargs='+', help="Path(s) to text log file(s).")
    parser.add_argument("--start", required=True, help="Start datetime (\"YYYY-MM-DD HH:MM:SS\").")
    parser.add_argument("--end", required=True, help="End datetime (\"YYYY-MM-DD HH:MM:SS\").")
    parser.add_argument("--interval", choices=["day", "hour"], default="day",
                        help="Graph granularity: 'day' or 'hour'. Default is 'day'.")
    return parser.parse_args()


def get_time_key(timestamp, interval):
    return timestamp.strftime("%Y-%m-%d %H:00") if interval == "hour" else timestamp.strftime("%Y-%m-%d")


def sanitize_for_filename(s):
    return re.sub(r"[^\w\-]", "_", s)


def analyze_logs(files, start_dt, end_dt, interval):
    rocket_ignite_counter = defaultdict(int)
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

                    if "The rocket has ignited" in line:
                        rocket_ignite_counter[time_key] += 1
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
        return None  # No data found

    return {
        "The rocket has ignited": rocket_ignite_counter,
        "Language: English": language_eng_counter,
        "Language: Hebrew": language_heb_counter,
        "Language: Arabic": language_arb_counter,
        "First Timestamp": first_dt,
        "Last Timestamp": last_dt,
    }


def write_summary(data_dict, interval, start_dt, end_dt):
    """
    Write a summary of the log analysis to a text file.
    Args:
        data_dict (dict): Dictionary containing counts of events.
        interval (str): Interval type ('day' or 'hour').
        start_dt (datetime): Start datetime for the analysis.
        end_dt (datetime): End datetime for the analysis.
    """

    def total(counter):
        return sum(counter.values())

    ring_total = total(data_dict["The rocket has ignited"])
    eng_total = total(data_dict["Language: English"])
    heb_total = total(data_dict["Language: Hebrew"])
    arb_total = total(data_dict["Language: Arabic"])
    first_dt = data_dict["First Timestamp"]
    last_dt = data_dict["Last Timestamp"]

    # remove the first and last timestamps from the data_dict
    del data_dict["First Timestamp"]
    del data_dict["Last Timestamp"]

    # Count of distinct intervals (e.g., days or hours)
    total_intervals = len(set().union(*[d.keys() for d in data_dict.values()]))
    total_langs = eng_total + heb_total + arb_total

    ring_avg = ring_total / total_intervals if total_intervals else 0
    eng_avg = eng_total / total_intervals if total_intervals else 0
    heb_avg = heb_total / total_intervals if total_intervals else 0
    arb_avg = arb_total / total_intervals if total_intervals else 0
    total_langs_avg = total_langs / total_intervals if total_intervals else 0

    denom = ring_total + total_langs
    good_runs_pct = (ring_total / denom) * 100 if denom else 0

    # Prepare output path
    start_fmt = sanitize_for_filename(start_dt.strftime("%Y-%m-%d_%H-%M-%S"))
    end_fmt = sanitize_for_filename(end_dt.strftime("%Y-%m-%d_%H-%M-%S"))
    filename = f"Rocket_ignite_summary_{start_fmt}_{end_fmt}.txt"

    with open(filename, "w") as f:
        f.write(f"Summary from {start_dt} to {end_dt} ({(end_dt - start_dt).days + 1} days)\n")
        f.write(f"Log data time range: {first_dt} to {last_dt} ({(last_dt - first_dt).days + 1} days)\n\n")

        f.write(f"Total Rocket ignite: {ring_total}\n")
        f.write(f"Total english language: {eng_total}\n")
        f.write(f"Total hebrew language: {heb_total}\n")
        f.write(f"Total arabic language: {arb_total}\n")
        f.write(f"Total language changes: {total_langs}\n\n")

        f.write(f"Average Rocket ignite per {interval}: {ring_avg:.2f}\n")
        f.write(f"Average english language per {interval}: {eng_avg:.2f}\n")
        f.write(f"Average hebrew language per {interval}: {heb_avg:.2f}\n")
        f.write(f"Average arabic language per {interval}: {arb_avg:.2f}\n")
        f.write(f"Average total language changes per {interval}: {total_langs_avg:.2f}\n\n")

        f.write(f"Good runs: {ring_total} / {denom} ({good_runs_pct:.2f}%)\n")

    print(f"Saved summary to: {filename}")


def plot_and_save_counts(data_dict, interval, start_dt, end_dt):
    all_keys = sorted(set().union(*[counter.keys() for counter in data_dict.values()]))
    x = np.arange(len(all_keys))

    bar_width = 0.2
    offset = 0
    plt.figure(figsize=(12, 6))

    for label, counter in data_dict.items():
        counts = [counter.get(k, 0) for k in all_keys]
        plt.bar(x + offset, counts, width=bar_width, label=label)
        offset += bar_width

    plt.xticks(x + bar_width * 1.5, all_keys, rotation=45, ha="right")
    plt.ylabel("Count")
    plt.xlabel("Hour" if interval == "hour" else "Date")
    plt.title("Log Event Counts")
    plt.legend()
    plt.tight_layout()
    plt.grid(True, axis='y')

    # Save to file
    start_fmt = sanitize_for_filename(start_dt.strftime("%Y-%m-%d_%H-%M-%S"))
    end_fmt = sanitize_for_filename(end_dt.strftime("%Y-%m-%d_%H-%M-%S"))
    filename = f"Rocket_ignite_plot_{start_fmt}_{end_fmt}.png"
    plt.savefig(filename)
    print(f"Saved bar graph to: {filename}")


def confirm_large_hour_interval(start_dt, end_dt, interval):
    if interval == "hour":
        delta_days = (end_dt - start_dt).days
        if delta_days >= 6:
            print(
                f"⚠️  WARNING: Time range is {delta_days} days but interval is set to 'hour'. This may produce a large and cluttered graph.")
            proceed = input("Do you want to continue? (yes/no): ").strip().lower()
            if proceed not in ["yes", "y", "Y", "Yes"]:
                print("Aborted by user.")
                sys.exit(0)


def check_format_and_incoherance(dt_start, dt_end, interval):
    try:
        start_dt = datetime.strptime(dt_start, "%Y-%m-%d %H:%M:%S")
        end_dt = datetime.strptime(dt_end, "%Y-%m-%d %H:%M:%S")
    except ValueError:
        print("Invalid datetime format. Use 'YYYY-MM-DD HH:MM:SS'", file=sys.stderr)
        sys.exit(1)

    if start_dt >= end_dt:
        print("Start datetime must be earlier than end datetime.", file=sys.stderr)
        sys.exit(1)

    confirm_large_hour_interval(start_dt, end_dt, interval)
    return start_dt, end_dt, interval


def main():
    args = parse_args()
    start_dt, end_dt, interval = check_format_and_incoherance(args.start, args.end, args.interval)  # with terminal
    files = args.files

    # start_dt, end_dt, interval = check_format_and_incoherance(START_DT, END_DT, INTERVAL)  #set Mamualy
    # files = FILE_PATH

    data_dict = analyze_logs(files, start_dt, end_dt, interval)
    if not data_dict:
        print("No data found in the specified time range. No graph was generated.")
        sys.exit(0)

    plot_and_save_counts({k: v for k, v in data_dict.items() if "Timestamp" not in k}, interval, start_dt, end_dt)
    write_summary(data_dict, interval, start_dt, end_dt)


if __name__ == "__main__":
    main()
