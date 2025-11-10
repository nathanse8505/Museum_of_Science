import re

input_file = "log_modifier.txt"
output_file = "log_modifier1.txt"

with open(input_file, "r", encoding="utf-8") as infile:
    lines = infile.readlines()

new_lines = []

for i, line in enumerate(lines):
    # Remplacer underscore par espace entre date et heure
    line = re.sub(r"(\d{4}-\d{2}-\d{2})_(\d{2}:\d{2}:\d{2})", r"\1 \2", line)

    # Détection des lignes suspectes
    if "your horsepower is: 0.4" in line or "your horsepower is: 0.5" in line:
        prev_line = lines[i - 1] if i > 0 else ""
        if "Connected to /dev/ttyUSB0" not in prev_line:
            try:
                # Extraire le timestamp
                timestamp = line.split(" - ")[0].strip()
                new_lines.append(f"{timestamp} - reset Arduino\n")
            except Exception:
                new_lines.append("reset Arduino\n")
        else:
            new_lines.append(line)
    else:
        new_lines.append(line)

with open(output_file, "w", encoding="utf-8") as outfile:
    outfile.writelines(new_lines)

print(f"✅ Fichier traité enregistré sous : {output_file}")
