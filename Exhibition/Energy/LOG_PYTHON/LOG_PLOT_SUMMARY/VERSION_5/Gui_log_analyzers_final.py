import tkinter as tk
from tkinter import ttk, filedialog, messagebox
from tkcalendar import DateEntry
import os
from datetime import datetime

import matplotlib
matplotlib.use("TkAgg")
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

import generic_log_analysis_new as glan

lst_project = [
    "Rocket Hydrogen",
    "Horsepower",
    "Jumping Ring",
    "AirPressure",
    "Light a Fire",
    "Chliran",
    "Pendulum",
]


class LogAnalyzerGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Log Analyzer")
        self.root.protocol("WM_DELETE_WINDOW", self.on_close)

        # états
        self.log_file_paths = []
        self.start_date = tk.StringVar()
        self.start_time = tk.StringVar(value="00:00:00")
        self.end_date = tk.StringVar()
        self.end_time = tk.StringVar(value="23:59:59")
        self.interval = tk.StringVar(value="day")
        self.exhibits = tk.StringVar(value="Horsepower")

        # ce qu'on garde du dernier run
        self.last_figure = None
        self.last_result = None
        self.last_project_name = None
        self.last_start_dt = None
        self.last_end_dt = None
        self.plot_window = None

        self.create_widgets()

    def create_widgets(self):
        padding = {'padx': 10, 'pady': 5}

        # fichier
        ttk.Label(self.root, text="Log File(s):").grid(row=0, column=0, sticky="w", **padding)
        self.log_file_path_label = ttk.Label(self.root, text="No file selected", width=50)
        self.log_file_path_label.grid(row=0, column=1, columnspan=2, sticky="w", **padding)
        ttk.Button(self.root, text="Browse...", command=self.browse_file).grid(row=0, column=2, **padding)

        # dates
        ttk.Label(self.root, text="Start Date:").grid(row=1, column=0, sticky="w", **padding)
        self.start_date_entry = DateEntry(self.root, textvariable=self.start_date, date_pattern='yyyy-mm-dd')
        self.start_date_entry.grid(row=1, column=1, sticky="w", **padding)

        ttk.Label(self.root, text="Hour start:").grid(row=1, column=2, sticky="e", **padding)
        ttk.Entry(self.root, textvariable=self.start_time, width=10).grid(row=1, column=3, sticky="w", **padding)

        ttk.Label(self.root, text="End Date:").grid(row=2, column=0, sticky="w", **padding)
        self.end_date_entry = DateEntry(self.root, textvariable=self.end_date, date_pattern='yyyy-mm-dd')
        self.end_date_entry.grid(row=2, column=1, sticky="w", **padding)

        ttk.Label(self.root, text="Hour end:").grid(row=2, column=2, sticky="e", **padding)
        ttk.Entry(self.root, textvariable=self.end_time, width=10).grid(row=2, column=3, sticky="w", **padding)

        # interval + exhibit
        ttk.Label(self.root, text="Interval:").grid(row=3, column=0, sticky="w", **padding)
        interval_menu = ttk.Combobox(self.root, textvariable=self.interval,
                                     values=["day", "hour"], state="readonly")
        interval_menu.grid(row=3, column=1, sticky="w", **padding)

        ttk.Label(self.root, text="Exhibits:").grid(row=3, column=2, sticky="e", **padding)
        exhibits_menu = ttk.Combobox(self.root, textvariable=self.exhibits,
                                     values=lst_project, state="readonly")
        exhibits_menu.grid(row=3, column=3, sticky="w", **padding)

        # bouton run
        ttk.Button(self.root, text="Run Analysis", command=self.run_analysis).grid(
            row=4, column=0, columnspan=4, pady=15
        )

        # bouton save
        self.save_button = ttk.Button(self.root, text="Save…", command=self.save_both)
        self.save_button.grid(row=5, column=0, columnspan=4, pady=5)
        self.save_button.state(["disabled"])

    def browse_file(self):
        paths = filedialog.askopenfilenames(
            title="Select log file(s)",
            filetypes=[("Text Files", "*.txt"), ("All Files", "*.*")]
        )
        if paths:
            self.log_file_paths = list(paths)
            self.log_file_path_label.config(text=f"{len(paths)} file(s) selected")

    def run_analysis(self):
        proj = self.exhibits.get()

        # reset
        self.last_figure = None
        self.last_result = None
        self.last_project_name = None
        self.last_start_dt = None
        self.last_end_dt = None
        self.save_button.state(["disabled"])

        # parse dates
        try:
            start_dt = datetime.strptime(self.start_date.get() + " " + self.start_time.get(), "%Y-%m-%d %H:%M:%S")
            end_dt = datetime.strptime(self.end_date.get() + " " + self.end_time.get(), "%Y-%m-%d %H:%M:%S")
        except ValueError:
            messagebox.showerror("Invalid Input", "Datetime format must be YYYY-MM-DD HH:MM:SS")
            return

        if not self.log_file_paths:
            messagebox.showerror("No file", "Please select at least one log file.")
            return

        # cas spéciaux
        if proj == "Chliran":
            try:
                from Chliran_log import main_chliran
            except ImportError:
                messagebox.showerror("Import error", "Module Chliran_log introuvable.")
                return
            for path in self.log_file_paths:
                main_chliran.analyze_chliran(path, start_dt, end_dt)
            self.try_show_current_matplotlib()
            return

        if proj == "Pendulum":
            try:
                from Pendulum_log import main_pendulum
            except ImportError:
                messagebox.showerror("Import error", "Module Pendulum_log introuvable.")
                return
            for path in self.log_file_paths:
                main_pendulum.analyze_pendulum(path, start_dt, end_dt)
            self.try_show_current_matplotlib()
            return

        # cas généraux
        event_config = {
            "Rocket Hydrogen": "The rocket has ignited",
            "Horsepower": "your horsepower is",
            "Jumping Ring": "Ring jumped!",
            "AirPressure": "The Bottle flew!",
            "Light a Fire": "Peak temperature reached:"
        }

        try:
            result = glan.analyze_logs(
                files=self.log_file_paths,
                start_dt=start_dt,
                end_dt=end_dt,
                interval=self.interval.get(),
                event_config=event_config,
                project_name=proj
            )
        except Exception as e:
            messagebox.showerror("Error", f"Error during analysis: {e}")
            return

        if not result:
            messagebox.showinfo("No Data", "No data found in the specified time range.")
            return

        # on crée le graphe (sans sauvegarde)
        fig = None
        try:
            fig = glan.plot_counts(result, self.interval.get())
        except AttributeError:
            try:
                fig = glan.plt_counts(result, self.interval.get())
            except Exception:
                fig = None

        # affiche
        self.handle_plot_result(fig)

        # on garde tout pour le save
        self.last_figure = fig
        self.last_result = result
        self.last_project_name = proj
        self.last_start_dt = start_dt
        self.last_end_dt = end_dt

        # activer le bouton save
        self.save_button.state(["!disabled"])

        # pas de messagebox ici, comme tu veux

    def handle_plot_result(self, fig):
        if fig is None:
            if plt.get_fignums():
                fig = plt.gcf()
            else:
                return

        # détruire ancienne fenêtre
        if self.plot_window is not None and tk.Toplevel.winfo_exists(self.plot_window):
            self.plot_window.destroy()

        self.plot_window = tk.Toplevel(self.root)
        self.plot_window.title("Analysis plot")
        canvas = FigureCanvasTkAgg(fig, master=self.plot_window)
        canvas.draw()
        canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)

    def try_show_current_matplotlib(self):
        if plt.get_fignums():
            fig = plt.gcf()
            self.handle_plot_result(fig)
            self.save_button.state(["!disabled"])

    def save_both(self):
        """
        1. L'utilisateur choisit un dossier parent
        2. On crée dedans un dossier {project}_{MM_DD}_to_{MM_DD}
        3. On met dedans:
           - {project}_{MM_DD}_to_{MM_DD}.png
           - summary_{project}_{MM_DD}-{MM_DD}.txt
        """
        if self.last_figure is None or self.last_result is None \
                or self.last_project_name is None \
                or self.last_start_dt is None or self.last_end_dt is None:
            messagebox.showerror("Save", "No analysis to save.")
            return

        proj = self.last_project_name
        start_dt = self.last_start_dt
        end_dt = self.last_end_dt

        start_md = start_dt.strftime("%d_%m")
        end_md = end_dt.strftime("%d_%m")

        # 1. choisir le dossier parent
        parent_dir = filedialog.askdirectory(
            title="Select folder where to create the analysis folder"
        )
        if not parent_dir:
            return

        # 2. créer le dossier à l'intérieur
        folder_name = f"{proj}_{start_md}_to_{end_md}"
        target_dir = os.path.join(parent_dir, folder_name)

        if os.path.exists(target_dir):
            # dossier existe déjà → demander si on l'utilise
            use_it = messagebox.askyesno(
                "Folder exists",
                f"The folder '{folder_name}' already exists.\nUse it and overwrite files inside?"
            )
            if not use_it:
                return
        else:
            try:
                os.makedirs(target_dir, exist_ok=True)
            except Exception as e:
                messagebox.showerror("Folder error", f"Could not create folder:\n{e}")
                return

        # 3. chemins des fichiers
        png_path = os.path.join(target_dir, f"{proj}_{start_md}_to_{end_md}.png")
        txt_path = os.path.join(target_dir, f"summary_{proj}_{start_md}-{end_md}.txt")

        # 4. si l'un des fichiers existe → demander
        exists_list = []
        if os.path.exists(png_path):
            exists_list.append(os.path.basename(png_path))
        if os.path.exists(txt_path):
            exists_list.append(os.path.basename(txt_path))

        if exists_list:
            listing = "\n".join(exists_list)
            ok = messagebox.askyesno(
                "Files exist",
                f"The following file(s) already exist in this folder:\n{listing}\nOverwrite?"
            )
            if not ok:
                return

        # 5. sauver le PNG
        try:
            self.last_figure.savefig(png_path)
        except Exception as e:
            messagebox.showerror("Save PNG", f"Could not save PNG:\n{e}")
            return

        # 6. sauver le TXT via le module
        try:
            glan.write_summary_to_file(
                self.last_result,
                self.interval.get(),
                start_dt,
                end_dt,
                txt_path
            )
            messagebox.showinfo("Save Complete", "The analysis and summary files have been saved successfully.")

        except Exception as e:
            messagebox.showerror("Save TXT", f"Could not save TXT:\n{e}")
            return

    def on_close(self):
        self.root.quit()
        self.root.destroy()


if __name__ == "__main__":
    root = tk.Tk()
    window_width = 650
    window_height = 260
    screen_width = root.winfo_screenwidth()
    screen_height = root.winfo_screenheight()
    x = int((screen_width - window_width) / 2)
    y = int((screen_height - window_height) / 2)
    root.geometry(f"{window_width}x{window_height}+{x}+{y}")

    app = LogAnalyzerGUI(root)
    root.mainloop()
