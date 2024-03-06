#!/bin/bash

PROCESS_NAME="./bin/server"

# Trouver le PID du processus
PID=$(pgrep -f $PROCESS_NAME)

# Vérifier si le PID a été trouvé
if [ ! -z "$PID" ]; then
    echo "Arrêt du processus serveur avec le PID $PID"
    kill -2 $PID
    echo "Processus arrêté."
else
    echo "Processus serveur non trouvé."
fi
