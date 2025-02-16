import csv
from datetime import datetime

# Chemins des fichiers
input_file = 'logs/log.txt'  # Fichier de log d'entrée
output_file = 'logs/log_filtered.csv'  # Fichier CSV de sortie

# Ouvrir le fichier de log en mode lecture
with open(input_file, 'r') as log_file:
    lines = log_file.readlines()

# Ouvrir le fichier CSV en mode écriture
with open(output_file, 'w', newline='') as csv_file:
    csv_writer = csv.writer(csv_file)

    # Écrire l'en-tête des colonnes
    csv_writer.writerow([
        'Date', 'Time', 'Language Message',  # Colonnes pour "language"
        'Date', 'Time', 'Horsepower Message'  # Colonnes pour "horsepower"
    ])

    # Initialiser des variables pour stocker les données
    language_data = {'date': '', 'time': '', 'message': ''}
    horsepower_data = {'date': '', 'time': '', 'message': ''}

    # Parcourir chaque ligne du fichier de log
    for line in lines:
        # Supprimer les espaces inutiles et diviser la ligne en deux parties : timestamp et message
        line = line.strip()
        if ' - ' in line:
            timestamp, message = line.split(' - ', 1)

            # Convertir le timestamp en date et heure
            try:
                dt = datetime.strptime(timestamp, '%Y-%m-%d %H:%M:%S')
                date = dt.strftime('%Y-%m-%d')
                time = dt.strftime('%H:%M:%S')

                # Vérifier si le message contient "language" ou "horsepower"
                if 'language' in message.lower():
                    language_data = {'date': date, 'time': time, 'message': message}
                elif 'horsepower' in message.lower():
                    horsepower_data = {'date': date, 'time': time, 'message': message}

                # Écrire la ligne dans le fichier CSV si l'un des messages est non vide
                if language_data['message'] or horsepower_data['message']:
                    csv_writer.writerow([
                        language_data['date'], language_data['time'], language_data['message'],
                        horsepower_data['date'], horsepower_data['time'], horsepower_data['message']
                    ])
                    # Réinitialiser les données après écriture
                    language_data = {'date': '', 'time': '', 'message': ''}
                    horsepower_data = {'date': '', 'time': '', 'message': ''}
            except ValueError:
                print(f"Erreur de format de timestamp dans la ligne : {line}")

print(f"Le fichier CSV filtré '{output_file}' a été créé avec succès.")