import tkinter as tk
from tkinter import ttk, filedialog, messagebox
from tkcalendar import DateEntry
from datetime import datetime, time
import os

class LogAnalyzerGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Log Analyzer")
        self.root.protocol("WM_DELETE_WINDOW", self.on_close)

        self.log_file_path = tk.StringVar()
        self.start_date = tk.StringVar()
        self.start_time = tk.StringVar()
        self.end_date = tk.StringVar()
        self.end_time = tk.StringVar()
        self.interval = tk.StringVar(value="day")

        self.create_widgets()

    def create_widgets(self):
        padding = {'padx': 10, 'pady': 5}

        # File selector
        ttk.Label(self.root, text="Log File:").grid(row=0, column=0, sticky="w", **padding)
        ttk.Entry(self.root, textvariable=self.log_file_path, width=50).grid(row=0, column=1, **padding)
        ttk.Button(self.root, text="Browse...", command=self.browse_file).grid(row=0, column=2, **padding)

        # Start datetime
        ttk.Label(self.root, text="Start Date:").grid(row=1, column=0, sticky="w", **padding)
        self.start_date_entry = DateEntry(self.root, textvariable=self.start_date, date_pattern='yyyy-mm-dd')
        self.start_date_entry.grid(row=1, column=1, sticky="w", **padding)
        ttk.Entry(self.root, textvariable=self.start_time, width=10).grid(row=1, column=2, sticky="w", **padding)
        self.start_time.set("00:00:00")

        # End datetime
        ttk.Label(self.root, text="End Date:").grid(row=2, column=0, sticky="w", **padding)
        self.end_date_entry = DateEntry(self.root, textvariable=self.end_date, date_pattern='yyyy-mm-dd')
        self.end_date_entry.grid(row=2, column=1, sticky="w", **padding)
        ttk.Entry(self.root, textvariable=self.end_time, width=10).grid(row=2, column=2, sticky="w", **padding)
        self.end_time.set("23:59:59")

        # Interval dropdown
        ttk.Label(self.root, text="Interval:").grid(row=3, column=0, sticky="w", **padding)
        interval_menu = ttk.Combobox(self.root, textvariable=self.interval, values=["day", "hour"], state="readonly")
        interval_menu.grid(row=3, column=1, sticky="w", **padding)

        # Run button
        ttk.Button(self.root, text="Run Analysis", command=self.run_analysis).grid(row=4, column=0, columnspan=3, pady=15)

    def browse_file(self):
        path = filedialog.askopenfilename(title="Select log file", filetypes=[("Text Files", "*.txt"), ("All Files", "*.*")])
        if path:
            self.log_file_path.set(path)

    def run_analysis(self):
        file_path = self.log_file_path.get()
        try:
            start_dt = datetime.strptime(self.start_date.get() + " " + self.start_time.get(), "%Y-%m-%d %H:%M:%S")
            end_dt = datetime.strptime(self.end_date.get() + " " + self.end_time.get(), "%Y-%m-%d %H:%M:%S")
        except ValueError:
            messagebox.showerror("Invalid Input", "Datetime format must be YYYY-MM-DD HH:MM:SS")
            return

        if not os.path.isfile(file_path):
            messagebox.showerror("Invalid File", "Selected log file does not exist.")
            return

        if start_dt >= end_dt:
            messagebox.showerror("Invalid Range", "Start datetime must be earlier than end datetime.")
            return

        # Import your main logic and run analysis here
        import log_analysis_main  # your analysis script renamed as a module
        result = log_analysis_main.analyze_logs([file_path], start_dt, end_dt, self.interval.get())

        if not result:
            messagebox.showinfo("No Data", "No data found in the specified time range.")
        else:
            log_analysis_main.plot_and_save_counts({k: v for k, v in result.items() if "Timestamp" not in k}, self.interval.get(), start_dt, end_dt)
            log_analysis_main.write_summary(result, self.interval.get(), start_dt, end_dt)
            messagebox.showinfo("Done", "Analysis complete. Output files saved.")

    def on_close(self):
        # Ici, tu peux faire d'autres nettoyages si besoin
        print("Fermeture de l'application par l'utilisateur.")
        self.root.quit()
        self.root.destroy()


if __name__ == "__main__":
    root = tk.Tk()
    app = LogAnalyzerGUI(root)
    root.mainloop()