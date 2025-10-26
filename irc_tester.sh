#!/bin/sh

# ----------------------------------------
# ft_irc test suite complet (USER uses $CLIENT_user)
# by drhaouha
# ----------------------------------------

# --- Usage
echo "[USAGE] ./irc_test.sh optional[<port> <password>]"

# --- Paramètres par défaut ---
PORT=${1:-6667}
PASSWORD=${2:-1234}

SERVER=127.0.0.1
IRCSERV=./ircserv


# -------------------------
# Fonction client scripté
# USER uses CLIENT + _user to differ from NICK
# -------------------------
run_client() {
	CLIENT="$1"
	shift
	USERNAME="${CLIENT}_user"
	{
		echo -n "PASS $PASSWORD\r\n"
		sleep 0.5
		echo -n "NICK $CLIENT\r\n"
		sleep 0.5
		echo -n "USER $USERNAME 0 * :Real $CLIENT\r\n"
		sleep 0.5
		for cmd in "$@"; do
			echo -n "$cmd\r\n"
			sleep 0.3
		done
		echo -n "QUIT :Bye\r\n"
	} | nc $SERVER $PORT
}

# ------------------------
# TEST 1: Connexion simple
# ------------------------
echo "\n"
echo "🧪 TEST 1: Basic connection"
run_client Alice

# ------------------------
# TEST 2: Join & message
# ------------------------
echo "\n"
echo "🧪 TEST 2: Join & PRIVMSG"
run_client Bob \
	"JOIN #42" \
	"PRIVMSG #42 :Hello from Bob"

# ------------------------
# TEST 3: Topic
# ------------------------
echo "\n"
echo "🧪 TEST 3: Topic"
run_client Carol \
	"JOIN #42" \
	"TOPIC #42 :New Topic by Carol" \
	"TOPIC #42"

# ------------------------
# TEST 4: Modes i, k, l, o, t
# ------------------------
echo "\n"
echo "🧪 TEST 4: Modes (i,k,l,o,t)"
# Garder Bob dans le canal pour les tests sur +o (background client)
# {
#     echo -n "PASS $PASSWORD\r\n"
#     echo -n "NICK Bob\r\n"
#     echo -n "USER Bob_user 0 * :Real Bob\r\n"
#     sleep 0.5
#     echo -n "JOIN #42\r\n"
#     sleep 9999
# } | nc -q 5 $SERVER $PORT &

timeout 30s sh -c '
	echo -n "PASS $PASSWORD\r\n"
	echo -n "NICK Bob\r\n"
	echo -n "USER Bob_user 0 * :Real Bob\r\n"
	sleep 0.5
	echo -n "JOIN #42\r\n"
	sleep 9999
' | nc $SERVER $PORT &
BOB_PID=$!
sleep 0.6

echo "\n"

run_client Dave \
	"JOIN #42" \
	"MODE #42 +i" \
	"MODE #42 +k secret" \
	"MODE #42 -k" \
	"MODE #42 +l 2" \
	"MODE #42 -l" \
	"MODE #42 +t" \
	"TOPIC #42 :Topic par Dave" \
	"MODE #42 +o Bob" \
	"MODE #42 -o Bob"

kill $BOB_PID 2>/dev/null || true
wait $BOB_PID 2>/dev/null || true

# ------------------------
# TEST 5: JOIN avec clé
# ------------------------
echo "\n"
echo "🧪 TEST 5: JOIN avec clé"
# Créer un canal avec clé par KeyMaster (background client short-lived)
{
	echo -n "PASS $PASSWORD\r\n"
	echo -n "NICK KeyMaster\r\n"
	echo -n "USER KeyMaster_user 0 * :Key Master\r\n"
	sleep 0.5
	echo -n "JOIN #secure\r\n"
	echo -n "MODE #secure +k secret\r\n"
	sleep 3
	echo -n "QUIT :Bye\r\n"
} | nc $SERVER $PORT &

sleep 1

# Client sans clé -> doit échouer
run_client Eve "JOIN #secure"

# Client avec clé -> doit réussir
run_client Frank "JOIN #secure secret"

# ------------------------
# TEST 6: WHO / WHOIS / NAMES
# ------------------------
echo "\n"
echo "🧪 TEST 6: WHO, WHOIS, NAMES"
run_client Grace \
	"JOIN #42" \
	"WHO #42" \
	"NAMES #42" \
	"WHOIS Bob"

# ------------------------
# TEST 7: Erreurs
# ------------------------
echo "\n"
echo "🧪 TEST 7: Invalid commands"
run_client Henry \
	"NICK" \
	"USER" \
	"JOIN" \#!/bin/sh