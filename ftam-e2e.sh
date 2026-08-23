#!/usr/bin/env bash
# Smoke-test native FTAM: start tsapd on a high TCP port, then use ftam to
# write a file, list it, read it back, and query a name that does not exist.
#
# This does not require ISODE to be installed. It uses the binaries built in
# this source tree and a throwaway tree under .ftam-e2e/. bubblewrap overlays
# that tree onto /usr/local/etc/isode and replaces /etc/passwd with a copy
# whose test-user hash is known, so tsapd can find iso.ftam and ftamd can
# authenticate without root or the caller's real password.
#
# That directory is removed after a successful run and left in place (with
# logs and the filestore) if anything fails.
#
# Usage: ./ftam-e2e.sh
# Optional: FTAM_E2E_PORT (default 21704, to stay off tsapd's usual TCP 102)

set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKDIR="${FTAM_E2E_DIR:-$ROOT/.ftam-e2e}"
PORT="${FTAM_E2E_PORT:-21704}"
PASSWORD="secret"
REMOTE_FILE="ftam-e2e-hello.txt"
MISSING_FILE="ftam-e2e-missing-no-such-file"
PAYLOAD="MAJOR LEAGUE SWAGOUT"
ISOMACROS="$ROOT/support/isomacros"
ISOBJECTS="$ROOT/support/isobjects"
ISODOCUMENTS="$ROOT/ftam/isodocuments"

FTAM=""
FTAMD=""
TSAPD=""
TSAPD_PID=""
KEEP_WORKDIR=1
STARTED=0
USER_NAME="$(id -un)"
UID_N="$(id -u)"
GID_N="$(id -g)"
USR_LOCAL_ETC=""
ETCDIR=""
FILESTORE=""

die() {
	echo "ftam-e2e: $*" >&2
	exit 1
}

find_binaries() {
	local cand

	command -v bwrap >/dev/null 2>&1 \
		|| die "bubblewrap (bwrap) is required to overlay ISODE config without installing it"
	command -v openssl >/dev/null 2>&1 \
		|| die "openssl is required to generate the test password hash"

	for cand in "$ROOT/ftam2/xftam" "$ROOT/ftam2/ftam"; do
		if [[ -x "$cand" ]]; then
			FTAM="$cand"
			break
		fi
	done
	for cand in "$ROOT/ftam2/xftamd" "$ROOT/ftam2/ftamd"; do
		if [[ -x "$cand" ]]; then
			FTAMD="$cand"
			break
		fi
	done
	for cand in "$ROOT/support/xtsapd" "$ROOT/support/tsapd"; do
		if [[ -x "$cand" ]]; then
			TSAPD="$cand"
			break
		fi
	done

	[[ -n "$FTAM" ]] || die "FTAM client not found (build with './make everything' first)"
	[[ -n "$FTAMD" ]] || die "FTAM responder not found (build with './make everything' first)"
	[[ -n "$TSAPD" ]] || die "tsapd not found (build with './make everything' first)"
	[[ -f "$ISOMACROS" ]] || die "ISODE macros missing at $ISOMACROS"
	[[ -f "$ISOBJECTS" ]] || die "ISODE objects missing at $ISOBJECTS"
	[[ -f "$ISODOCUMENTS" ]] || die "FTAM documents table missing at $ISODOCUMENTS"
}

sandbox() {
	local extra=()
	if [[ "${1:-}" == "--new-session" ]]; then
		extra+=(--new-session)
		shift
	fi
	bwrap --dev-bind / / \
		--bind "$USR_LOCAL_ETC" /usr/local/etc \
		--bind "$WORKDIR/passwd" /etc/passwd \
		--uid "$UID_N" \
		--gid "$GID_N" \
		--die-with-parent \
		"${extra[@]}" \
		-- "$@"
}

stop_tsapd() {
	local pid="" extra
	if [[ -n "${TSAPD_PID:-}" ]]; then
		pid="$TSAPD_PID"
	fi
	if [[ -n "$pid" ]] && kill -0 "$pid" 2>/dev/null; then
		kill -TERM "$pid" 2>/dev/null || true
		local i
		for i in $(seq 1 20); do
			kill -0 "$pid" 2>/dev/null || break
			sleep 0.1
		done
		kill -KILL "$pid" 2>/dev/null || true
	fi
	extra="$(pgrep -f "${TSAPD} .*-p ${PORT}" || true)"
	if [[ -n "$extra" ]]; then
		# shellcheck disable=SC2086
		kill -TERM $extra 2>/dev/null || true
		sleep 0.2
		# shellcheck disable=SC2086
		kill -KILL $extra 2>/dev/null || true
	fi
}

on_exit() {
	local status=$?
	if [[ "$STARTED" -eq 1 ]]; then
		stop_tsapd
	fi
	if [[ "$status" -eq 0 && "$KEEP_WORKDIR" -eq 0 ]]; then
		rm -rf "$WORKDIR"
	elif [[ "$status" -ne 0 ]]; then
		echo "ftam-e2e: failed; leaving $WORKDIR for inspection" >&2
	fi
}

write_passwd() {
	local hash
	hash="$(openssl passwd -6 -salt ftame2e "$PASSWORD")"
	[[ -n "$hash" && "$hash" != "$PASSWORD" ]] \
		|| die "openssl did not return a password hash"
	awk -F: -v u="$USER_NAME" -v h="$hash" -v d="$FILESTORE" \
		'BEGIN { OFS=":" } $1 == u { $2 = h; $6 = d } { print }' \
		/etc/passwd > "$WORKDIR/passwd"
	grep -q "^${USER_NAME}:" "$WORKDIR/passwd" \
		|| die "could not copy the current user into the test passwd file"
}

write_configs() {
	cp "$ISOMACROS" "$ETCDIR/isomacros"
	cp "$ISOBJECTS" "$ETCDIR/isobjects"
	cp "$ISODOCUMENTS" "$ETCDIR/isodocuments"

	cat > "$ETCDIR/isoservices" <<EOF
"tsap/filestore"		#259		${FTAMD} -d
EOF

	cat > "$ETCDIR/isoentities" <<EOF
default		filestore	1.17.4.0.16	#259/localHost=${PORT}
EOF

	cat > "$ETCDIR/isotailor" <<EOF
etcpath:	${ETCDIR}/
logpath:	${WORKDIR}/
compatlevel:	exceptions
compatfile:	compat.log
addrlevel:	exceptions
addrfile:	addr.log
tsaplevel:	all
tsapfile:	tsapd.log
ssaplevel:	exceptions
ssapfile:	ssap.log
psaplevel:	exceptions
psapfile:	psap.log
psap2level:	exceptions
psap2file:	psap2.log
acsaplevel:	exceptions
acsapfile:	acsap.log
rtsaplevel:	exceptions
rtsapfile:	rtsap.log
rosaplevel:	exceptions
rosapfile:	rosap.log
EOF

	# Client-side backup if argv[0] is ftam or xftam.
	cat > "$WORKDIR/.xftam_tailor" <<EOF
etcpath:	/usr/local/etc/isode/
logpath:	${WORKDIR}/
EOF
	cp "$WORKDIR/.xftam_tailor" "$WORKDIR/.ftam_tailor"
	cp "$ISOMACROS" "$WORKDIR/.isode_macros"
}

prepare_workdir() {
	if [[ -d "$WORKDIR" ]]; then
		echo "ftam-e2e: replacing leftover $WORKDIR"
	fi
	TSAPD="${TSAPD:-$ROOT/support/xtsapd}"
	stop_tsapd
	rm -rf "$WORKDIR"
	USR_LOCAL_ETC="$WORKDIR/usr-local-etc"
	ETCDIR="$USR_LOCAL_ETC/isode"
	FILESTORE="$WORKDIR/filestore"
	mkdir -p "$ETCDIR" "$FILESTORE"
	write_passwd
	write_configs
	printf '%s\n' "$PAYLOAD" > "$WORKDIR/payload.txt"
	export HOME="$WORKDIR"
	export USER="$USER_NAME"
	export LOGNAME="$USER_NAME"
	# Local put/get paths are relative to the client's cwd.
	cd "$WORKDIR"
}

port_is_up() {
	# Prefer a listen-table check over a real TCP connect.
	if command -v ss >/dev/null 2>&1; then
		ss -ltn | grep -qE ":${PORT}[[:space:]]"
	else
		timeout 0.2 bash -c "echo >/dev/tcp/127.0.0.1/${PORT}" >/dev/null 2>&1
	fi
}

start_tsapd() {
	echo "ftam-e2e: starting tsapd on TCP $PORT"
	sandbox "$TSAPD" -f -t -r -p "$PORT" \
		>"$WORKDIR/tsapd.stdout" 2>"$WORKDIR/tsapd.stderr" &
	TSAPD_PID=$!
	STARTED=1

	local i
	for i in $(seq 1 80); do
		if ! kill -0 "$TSAPD_PID" 2>/dev/null; then
			echo "ftam-e2e: tsapd exited during startup" >&2
			if [[ -s "$WORKDIR/tsapd.stderr" ]]; then
				echo "ftam-e2e: tsapd.stderr:" >&2
				cat "$WORKDIR/tsapd.stderr" >&2
			fi
			return 1
		fi
		if port_is_up; then
			echo "ftam-e2e: tsapd is listening (pid $TSAPD_PID)"
			return 0
		fi
		sleep 0.25
	done

	echo "ftam-e2e: tsapd did not listen on $PORT in time" >&2
	if [[ -s "$WORKDIR/tsapd.stderr" ]]; then
		echo "ftam-e2e: tsapd.stderr:" >&2
		cat "$WORKDIR/tsapd.stderr" >&2
	fi
	if [[ -s "$WORKDIR/tsapd.log" ]]; then
		echo "ftam-e2e: last lines of tsapd.log:" >&2
		tail -n 40 "$WORKDIR/tsapd.log" >&2
	fi
	return 1
}

run_round_trip() {
	local out="$WORKDIR/ftam-roundtrip.out"
	local err="$WORKDIR/ftam-roundtrip.err"
	local got="$WORKDIR/downloaded.txt"
	local script="$WORKDIR/ftam-roundtrip.cmd"

	echo "ftam-e2e: putting, listing, and getting $REMOTE_FILE"
	{
		printf '%s\n' "$PASSWORD"
		echo "set query off"
		echo "set glob off"
		echo "set type text"
		echo "set realstore unix"
		echo "lcd $WORKDIR"
		echo "put payload.txt $REMOTE_FILE"
		echo "ls $REMOTE_FILE"
		echo "get $REMOTE_FILE downloaded.txt"
		echo "quit"
	} > "$script"

	set +e
	sandbox --new-session "$FTAM" -f -v -u "$USER_NAME" localhost \
		<"$script" >"$out" 2>"$err"
	local st=$?
	set -e

	if [[ "$st" -ne 0 ]]; then
		echo "ftam-e2e: FTAM round-trip command failed (status $st)" >&2
		echo "ftam-e2e: ftam stdout:" >&2
		cat "$out" >&2 || true
		echo "ftam-e2e: ftam stderr:" >&2
		cat "$err" >&2 || true
		return 1
	fi
	if grep -qiE 'unable to associate|not associated|localhost\.\.\. failed|service not found|NULLPA|rejected' "$out" "$err"; then
		echo "ftam-e2e: FTAM association/transfer reported an error" >&2
		cat "$out" "$err" >&2 || true
		return 1
	fi
	if ! grep -q "$REMOTE_FILE" "$out"; then
		echo "ftam-e2e: ls did not mention $REMOTE_FILE" >&2
		echo "ftam-e2e: ftam stdout:" >&2
		cat "$out" >&2 || true
		echo "ftam-e2e: ftam stderr:" >&2
		cat "$err" >&2 || true
		return 1
	fi
	if [[ ! -f "$got" ]]; then
		echo "ftam-e2e: downloaded file missing at $got" >&2
		cat "$out" "$err" >&2 || true
		return 1
	fi
	if ! cmp -s "$WORKDIR/payload.txt" "$got"; then
		echo "ftam-e2e: downloaded file does not match what was put" >&2
		echo "ftam-e2e: expected:" >&2
		cat "$WORKDIR/payload.txt" >&2
		echo "ftam-e2e: got:" >&2
		cat "$got" >&2
		return 1
	fi
	if [[ ! -f "$FILESTORE/$REMOTE_FILE" ]]; then
		echo "ftam-e2e: responder did not store $FILESTORE/$REMOTE_FILE" >&2
		return 1
	fi

	echo "ftam-e2e: downloaded file contents:" >&2
    cat downloaded.txt
	echo "ftam-e2e: round-trip ok"
	sed -n '1,80p' "$out"
}

run_missing() {
	local out="$WORKDIR/ftam-missing.out"
	local err="$WORKDIR/ftam-missing.err"
	local dest="$WORKDIR/should-not-exist.txt"
	local script="$WORKDIR/ftam-missing.cmd"

	echo "ftam-e2e: getting $MISSING_FILE (should fail)"
	rm -f "$dest"
	{
		printf '%s\n' "$PASSWORD"
		echo "set query off"
		echo "set glob off"
		echo "get $MISSING_FILE should-not-exist.txt"
		echo "quit"
	} > "$script"

	set +e
	sandbox --new-session "$FTAM" -f -v -u "$USER_NAME" localhost \
		<"$script" >"$out" 2>"$err"
	set -e

	if grep -qiE 'unable to associate|not associated|connect request refused|localhost\.\.\. failed' "$out" "$err"; then
		echo "ftam-e2e: FTAM did not associate for the missing-file check" >&2
		cat "$out" "$err" >&2 || true
		return 1
	fi
	if [[ -e "$dest" ]]; then
		echo "ftam-e2e: get of a missing file created $dest" >&2
		cat "$out" "$err" >&2 || true
		return 1
	fi
	if ! grep -qiE 'not exist|no such|filenot|does not|select.*fail|filename not found' "$out" "$err"; then
		echo "ftam-e2e: get of a missing file did not report an error" >&2
		echo "ftam-e2e: ftam stdout:" >&2
		cat "$out" >&2 || true
		echo "ftam-e2e: ftam stderr:" >&2
		cat "$err" >&2 || true
		return 1
	fi

	echo "ftam-e2e: missing-file error ok"
	sed -n '1,80p' "$out"
	if [[ -s "$err" ]]; then
		sed -n '1,40p' "$err"
	fi
}

trap on_exit EXIT

find_binaries
prepare_workdir
start_tsapd
run_round_trip
run_missing
KEEP_WORKDIR=0
echo "ftam-e2e: ok"
