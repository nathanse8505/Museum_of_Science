# === CONFIGURATION ===
input_file = "LOG.TXT"     # Remplace par le chemin de ton fichier
output_file = "LOG_modified.TXT"  # Le nouveau fichier sans les lignes à supprimer

# === TRAITEMENT ===
with open(input_file, 'r', encoding='utf-8') as f_in, open(output_file, 'w', encoding='utf-8') as f_out:
    for line in f_in:
        if "SD card successfully initialized." not in line:
            f_out.write(line)

print("Les lignes ont été filtrées avec succès.")
