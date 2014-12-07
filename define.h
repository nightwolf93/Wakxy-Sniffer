#ifndef DEFINE_H
#define DEFINE_H

//define setting file
#define SETTINGS_FILE "settings.ini"

//define default value of config file
#define SETTINGS_DEFAULT_AUTH_SERVER "80.239.173.156"
#define SETTINGS_DEFAULT_AUTH_PORT 5558

//define all usefull text for ui
#define TXT_UI_LABEL_SERVER "Proxy pour l'adresse "

#define TXT_UI_BUTTON_START_PROXY "Démarrer le proxy"
#define TXT_UI_BUTTON_STOP_PROXY "Arrêter le proxy"

#define TXT_UI_BUTTON_START_CAPTURE "Démarrer la capture"
#define TXT_UI_BUTTON_STOP_CAPTURE "Arrêter la capture"

//define all usefull text for console
#define TXT_LOG_DEFAULT "Bienvenue sur Wakxy Sniffer !"
#define TXT_LOG_RELOAD_SETTINGS "Rechargement des fichiers de configuration ! Le proxy et la capture va être arrêté"

#define TXT_LOG_PROXY_START "Proxy démarré"
#define TXT_LOG_PROXY_STOP "Proxy arreté"
#define TXT_LOG_PROXY_CONNEXION "Proxy connection"

#define TXT_LOG_REMOTE_CONNECT "Serveur distant connecté"
#define TXT_LOG_REMOTE_DISCONNECT "Serveur distant déconnecté"
#define TXT_LOG_REMOTE_PACKET_SEND "Serveur distant envoie un packet au client"

#define TXT_LOG_LOCAL_CONNECT "Local connecté"
#define TXT_LOG_LOCAL_DISCONNECT "Local déconnecté"
#define TXT_LOG_LOCAL_PACKET_SEND "Le client envoie un packet au serveur distant"


#endif // DEFINE_H
