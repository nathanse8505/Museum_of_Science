import tkinter as tk
from tkinter import ttk, filedialog, messagebox
from tkcalendar import DateEntry
from generic_log_analysis_new import *

lst_projקct = ["Rocket Hydrogen","Horsepower","Jumping Ring","AirPressure","Light a Fire","Chliran","Pendulum"]

class LogAnalyzerGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Log Analyzer")
        self.root.protocol("WM_DELETE_WINDOW", self.on_close)

        self.log_file_paths = []
        self.start_date = tk.StringVar()
        self.start_time = tk.StringVar()
        self.end_date = tk.StringVar()
        self.end_time = tk.StringVar()
        self.interval = tk.StringVar(value="day")
        self.exhibits = tk.StringVar(value="Horsepower")

        self.create_widgets()

    def create_widgets(self):
        padding = {'padx': 10, 'pady': 5}

        ttk.Label(self.root, text="Log File(s):").grid(row=0, column=0, sticky="w", **padding)
        self.log_file_path_label = ttk.Label(self.root, text="No file selected", width=50)
        self.log_file_path_label.grid(row=0, column=1, **padding)

        ttk.Button(self.root, text="Browse...", command=self.browse_file).grid(row=0, column=1, **padding)

        ttk.Label(self.root, text="Start Date:").grid(row=1, column=0, sticky="w", **padding)
        self.start_date_entry = DateEntry(self.root, textvariable=self.start_date, date_pattern='yyyy-mm-dd')
        self.start_date_entry.grid(row=1, column=1, sticky="w", **padding)

        ttk.Label(self.root, text="Hour start:").grid(row=1, column=1, sticky="e", **padding)
        ttk.Entry(self.root, textvariable=self.start_time, width=10).grid(row=1, column=2, sticky="w", **padding)
        self.start_time.set("00:00:00")

        ttk.Label(self.root, text="End Date:").grid(row=2, column=0, sticky="w", **padding)
        self.end_date_entry = DateEntry(self.root, textvariable=self.end_date, date_pattern='yyyy-mm-dd')
        self.end_date_entry.grid(row=2, column=1, sticky="w", **padding)

        ttk.Label(self.root, text="Hour end:").grid(row=2, column=1, sticky="e", **padding)
        ttk.Entry(self.root, textvariable=self.end_time, width=10).grid(row=2, column=2, sticky="w", **padding)
        self.end_time.set("23:59:59")

        ttk.Label(self.root, text="Interval:").grid(row=3, column=0, sticky="w", **padding)
        interval_menu = ttk.Combobox(self.root, textvariable=self.interval, values=["day", "hour"], state="readonly")
        interval_menu.grid(row=3, column=1, sticky="w", **padding)

        ttk.Label(self.root, text="Exhibits:").grid(row=3, column=1, sticky="e", **padding)
        interval_exhibits = ttk.Combobox(self.root, textvariable=self.exhibits, values=lst_projקct, state="readonly")
        interval_exhibits.grid(row=3, column=2, sticky="w", **padding)

        ttk.Button(self.root, text="Run Analysis", command=self.run_analysis).grid(row=4, column=0, columnspan=3,
                                                                                   pady=15)

    def browse_file(self):
        paths = filedialog.askopenfilenames(title="Select log file(s)",
                                            filetypes=[("Text Files", "*.txt"), ("All Files", "*.*")])
        if paths:
            self.log_file_paths = list(paths)
            self.log_file_path_label.config(text=f"{len(paths)} file(s) selected")

    def run_analysis(self):
        try:
            start_dt = datetime.strptime(self.start_date.get() + " " + self.start_time.get(), "%Y-%m-%d %H:%M:%S")
            end_dt = datetime.strptime(self.end_date.get() + " " + self.end_time.get(), "%Y-%m-%d %H:%M:%S")
        except ValueError:
            messagebox.showerror("Invalid Input", "Datetime format must be YYYY-MM-DD HH:MM:SS")
            return

        if not self.log_file_paths:
            messagebox.showerror("No File", "Please select at least one log file.")
            return

        for path in self.log_file_paths:
            if not os.path.isfile(path):
                messagebox.showerror("Invalid File", f"File does not exist: {path}")
                return

        event_config = {
            "Rocket Hydrogen": "The rocket has ignited",
            "Horsepower": "your horsepower is",
            "Jumping Ring": "Ring jumped!",
            "AirPressure": "The Bottle flew!",
            "Light a Fire": "Peak temperature reached:"
        }

        result = analyze_logs(
            files=self.log_file_paths,
            start_dt=start_dt,
            end_dt=end_dt,
            interval=self.interval.get(),
            event_config=event_config,
            project_name=detect_project_name(file_path=self.log_file_paths[0])
        )

        if not result:
            messagebox.showinfo("No Data", "No data found in the specified time range.")
        else:
            plot_and_save_counts(result, self.interval.get())
            write_summary(result, self.interval.get(), start_dt, end_dt)
            messagebox.showinfo("Done", "Analysis complete. Output files saved.")

    def on_close(self):
        print("Fermeture de l'application par l'utilisateur.")
        self.root.quit()
        self.root.destroy()


if __name__ == "__main__":
    root = tk.Tk()

    # === Centrer la fenêtre principale ===
    window_width = 570
    window_height = 200
    screen_width = root.winfo_screenwidth()
    screen_height = root.winfo_screenheight()
    x = int((screen_width - window_width) / 2)
    y = int((screen_height - window_height) / 2)
    root.geometry(f"{window_width}x{window_height}+{x}+{y}")

    app = LogAnalyzerGUI(root)
    root.mainloop()
