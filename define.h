#ifndef DEFINE_H
#define DEFINE_H

//define setting file
#define SETTINGS_FILE "settings.ini"

//define default value of config file
#define SETTINGS_DEFAULT_AUTH_SERVER "80.239.173.156"
#define SETTINGS_DEFAULT_AUTH_PORT 5558

#define SETTING_PACKETEDITOR_SCRIPT_FOLDER "./scripts"
#define SETTING_SPOOFING_DEFAULT_FOLDER "./spoofing"

//define all usefull text for ui
#define TXT_UI_LABEL_SERVER "Proxy pour l'adresse "

#define TXT_UI_BUTTON_START_PROXY "Démarrer le proxy"
#define TXT_UI_BUTTON_STOP_PROXY "Arrêter le proxy"

#define TXT_UI_BUTTON_START_CAPTURE "Démarrer la capture"
#define TXT_UI_BUTTON_STOP_CAPTURE "Arrêter la capture"

#define TXT_UI_ACTION_FILETYPE "Wakxy JSON file (*.wxy)"
#define TXT_UI_ACTION_SAVE_AS "Sauvegarder ..."
#define TXT_UI_ACTION_OPEN "Ouvrir ..."

#define TXT_UI_ACTION_SAVE_WARNING_FILENAME "Veuillez saisir un nom de fichier"
#define TXT_UI_ACTION_SAVE_WARNING_WRITE_FAIL "Impossible d'écrire dans le fichier"
#define TXT_UI_ACTION_SAVE_WARNING_READ_FAIL "Impossible de lire le fichier"

#define TXT_UI_LABEL_SCRIPT "Chemin du script : "
#define TXT_UI_LABEL_FREE_BYTE "Byte non lus : "

//define all usefull text for console
#define TXT_LOG_DEFAULT "Bienvenue sur Wakxy Sniffer !"
#define TXT_LOG_RELOAD_SETTINGS "Rechargement des fichiers de configuration ! Le proxy et la capture va être arrêté"

#define TXT_LOG_PROXY_START "Proxy démarré"
#define TXT_LOG_PROXY_STOP "Proxy arreté"
#define TXT_LOG_PROXY_CONNEXION "Connection au proxy"

#define TXT_LOG_CAPTURE_START "Capture démarré"
#define TXT_LOG_CAPTURE_STOP "Capture arreté"

#define TXT_LOG_REMOTE_CONNECT "Serveur distant connecté"
#define TXT_LOG_REMOTE_DISCONNECT "Serveur distant déconnecté"
#define TXT_LOG_REMOTE_PACKET_SEND "Serveur distant envoie un packet au client"
#define TXT_LOG_REMOTE_PACKET_SPOOFING "L'opcode %1 à été modifié avant d'étre envoyé au client"

#define TXT_LOG_LOCAL_CONNECT "Local connecté"
#define TXT_LOG_LOCAL_DISCONNECT "Local déconnecté"
#define TXT_LOG_LOCAL_PACKET_SEND "Le client envoie un packet au serveur distant"
#define TXT_LOG_LOCAL_PACKET_SPOOFING "L'opcode %1 à été modifié avant d'étre envoyé au serveur"

#define TXT_LOG_FILE_LOAD "Fichier chargé"
#define TXT_LOG_SPOOFING_LOADED "Le spoofing de l'opcode  %1 %2 est chargé"
#define TXT_LOG_SPOOFING_ENABLE "Le spoofing de l'opcode %1 %2 est activé"
#define TXT_LOG_SPOOFING_DISABLED "Le spoofing de l'opcode %1 %2 est désactivé"

#endif // DEFINE_H
