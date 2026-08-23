#!/usr/bin/env bash
# Smoke-test Quipu: start a local DSA, then use DISH to read its own entry.
#
# This does not require ISODE to be installed. It uses the binaries built in
# this source tree and a throwaway database under .quipu-e2e/.
#
# That directory is removed after a successful run and left in place (with
# logs, tailor files, and the EDB) if anything fails.
#
# Usage: ./quipu-e2e.sh
# Optional: QUIPU_E2E_PORT (default 21703, to stay off the usual Quipu 17003)

set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKDIR="${QUIPU_E2E_DIR:-$ROOT/.quipu-e2e}"
PORT="${QUIPU_E2E_PORT:-21703}"
DSA_NAME="cn=toucan"
DSA_PASSWORD="secret"
# localHost is defined in isomacros as 127.0.0.1. The macros are loaded from
# $HOME/.isode_macros so this works without installing ISODE.
PADDR="localHost=${PORT}"
OIDTABLE="$ROOT/dsap/oidtable"
ISOMACROS="$ROOT/support/isomacros"

QUIPU=""
DISH=""
DSA_PID=""
KEEP_WORKDIR=1
STARTED=0

die() {
	echo "quipu-e2e: $*" >&2
	exit 1
}

find_binaries() {
	local cand

	for cand in "$ROOT/quipu/xquipu" "$ROOT/quipu/quipu"; do
		if [[ -x "$cand" ]]; then
			QUIPU="$cand"
			break
		fi
	done
	for cand in "$ROOT/quipu/dish/xdish" "$ROOT/quipu/dish/dish"; do
		if [[ -x "$cand" ]]; then
			DISH="$cand"
			break
		fi
	done

	[[ -n "$QUIPU" ]] || die "Quipu DSA binary not found (build with './make everything' first)"
	[[ -n "$DISH" ]] || die "DISH binary not found (build with './make everything' first)"
	[[ -f "${OIDTABLE}.gen" && -f "${OIDTABLE}.at" && -f "${OIDTABLE}.oc" ]] \
		|| die "OID tables missing under $ROOT/dsap"
	[[ -f "$ISOMACROS" ]] || die "ISODE macros missing at $ISOMACROS"
}

stop_dsa() {
	local pid="" extra
	if [[ -n "${DSA_PID:-}" ]]; then
		pid="$DSA_PID"
	elif [[ -f "$WORKDIR/PID" ]]; then
		pid="$(tr -d '[:space:]' < "$WORKDIR/PID" || true)"
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
	# Listen-retry can leave a DSA with no PID file.
	extra="$(pgrep -f "${QUIPU} .*${WORKDIR}" || true)"
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
		stop_dsa
	fi
	if [[ "$status" -eq 0 && "$KEEP_WORKDIR" -eq 0 ]]; then
		rm -rf "$WORKDIR"
	elif [[ "$status" -ne 0 ]]; then
		echo "quipu-e2e: failed; leaving $WORKDIR for inspection" >&2
	fi
}

write_edb() {
	local now
	now="$(date -u +%y%m%d%H%M%SZ)"
	cat > "$WORKDIR/EDB" <<EOF
MASTER
${now}
${DSA_NAME}
objectClass= top & applicationEntity & dSA & quipuDSA
cn= toucan
description= End-to-end test DSA
presentationAddress= ${PADDR}
supportedApplicationContext= X500DSP & X500DAP & InternetDSP & quipuDSP
userPassword= ${DSA_PASSWORD}
manager= ${DSA_NAME}
acl= others # read # entry & others # read # default & others # read # child
eDBinfo= ##
quipuVersion= quipu e2e test
EOF
}

write_tailors() {
	cat > "$WORKDIR/quiputailor" <<EOF
oidtable	${OIDTABLE}
mydsaname	${DSA_NAME}
treedir		${WORKDIR}/
logdir		${WORKDIR}/
dsaplog		file=dsa.log level=all sflags=create
stats		file=dsa-stats.log sflags=create
update		off
dspchaining	off
EOF

	cat > "$WORKDIR/dsaptailor" <<EOF
oidtable	${OIDTABLE}
dsa_address	toucan		${PADDR}
dsaplog		file=dish.log level=exceptions sflags=create
stats		file=dish-stats.log sflags=create
oidformat	short
quipurc		off
sizelimit	20
EOF
}

prepare_workdir() {
	if [[ -d "$WORKDIR" ]]; then
		echo "quipu-e2e: replacing leftover $WORKDIR"
	fi
	# Stop any DSA still using this workdir before we wipe it.
	QUIPU="${QUIPU:-$ROOT/quipu/xquipu}"
	stop_dsa
	rm -rf "$WORKDIR"
	mkdir -p "$WORKDIR"
	# ISODE loads $HOME/.isode_macros after the (possibly missing) system file.
	cp "$ISOMACROS" "$WORKDIR/.isode_macros"
	write_edb
	write_tailors
	# Both Quipu and DISH read user files from HOME; keep them in the workdir.
	export HOME="$WORKDIR"
	unset DISHPROC || true
}

start_dsa() {
	echo "quipu-e2e: starting DSA $DSA_NAME on $PADDR"
	# Give the child a tty so listen failures return immediately instead of
	# retrying for five minutes, and so envinit does not double-fork.
	if command -v script >/dev/null 2>&1; then
		script -q -e -c "exec \"$QUIPU\" -r -t \"$WORKDIR/quiputailor\" -T \"$OIDTABLE\" -D \"$WORKDIR/\" -c \"$DSA_NAME\"" \
			"$WORKDIR/dsa.typescript" >/dev/null 2>&1 &
	else
		"$QUIPU" -r -t "$WORKDIR/quiputailor" \
			-T "$OIDTABLE" \
			-D "$WORKDIR/" \
			-c "$DSA_NAME" \
			>"$WORKDIR/dsa.stdout" 2>"$WORKDIR/dsa.stderr" &
	fi
	STARTED=1

	local i pid
	for i in $(seq 1 80); do
		if [[ -f "$WORKDIR/PID" ]]; then
			pid="$(tr -d '[:space:]' < "$WORKDIR/PID" || true)"
			if [[ -n "$pid" ]] && kill -0 "$pid" 2>/dev/null; then
				if grep -q 'has started on NULLPA' "$WORKDIR/dsa.log" 2>/dev/null; then
					echo "quipu-e2e: DSA started with a NULL presentation address" >&2
					return 1
				fi
				if grep -q 'has started on' "$WORKDIR/dsa.log" 2>/dev/null; then
					DSA_PID="$pid"
					echo "quipu-e2e: DSA is up (pid $DSA_PID)"
					return 0
				fi
			fi
		fi
		sleep 0.25
	done

	echo "quipu-e2e: DSA did not write a PID file in time" >&2
	if [[ -s "$WORKDIR/dsa.stderr" ]]; then
		echo "quipu-e2e: dsa.stderr:" >&2
		cat "$WORKDIR/dsa.stderr" >&2
	fi
	if [[ -s "$WORKDIR/dsa.log" ]]; then
		echo "quipu-e2e: last lines of dsa.log:" >&2
		tail -n 40 "$WORKDIR/dsa.log" >&2
	fi
	return 1
}

run_dish() {
	local out="$WORKDIR/dish.out" err="$WORKDIR/dish.err"
	local i

	echo "quipu-e2e: reading cn=toucan with DISH"
	# dish_init treats -tailor as unique from the first character and then
	# still increments the argv index, so -tailor must be the last option.
	# showentry (Read) is the directory operation: list of @ currently
	# comes back as a DAP "mistyped result" even when the DSA builds it.

	for i in $(seq 1 10); do
		set +e
		printf 'showentry @cn=toucan\nquit\n' | "$DISH" \
			-call toucan \
			-user "$DSA_NAME" \
			-password "$DSA_PASSWORD" \
			-simple \
			-tailor "$WORKDIR/dsaptailor" \
			>"$out" 2>"$err"
		local st=$?
		set -e
		if [[ "$st" -eq 0 ]]; then
			break
		fi
		if [[ "$i" -eq 10 ]]; then
			echo "quipu-e2e: DISH failed after retries" >&2
			echo "quipu-e2e: dish stdout:" >&2
			cat "$out" >&2 || true
			echo "quipu-e2e: dish stderr:" >&2
			cat "$err" >&2 || true
			return 1
		fi
		sleep 0.5
	done

	if grep -qiE 'unable to contact|service error|bind error|problem with dsa|mistyped' "$out" "$err"; then
		echo "quipu-e2e: DISH bind/read reported an error" >&2
		cat "$out" "$err" >&2 || true
		return 1
	fi
	if ! grep -qiE 'quipuDSA' "$out" || ! grep -qiE 'toucan' "$out"; then
		echo "quipu-e2e: showentry of the test DSA did not look right" >&2
		echo "quipu-e2e: dish stdout:" >&2
		cat "$out" >&2 || true
		echo "quipu-e2e: dish stderr:" >&2
		cat "$err" >&2 || true
		return 1
	fi
	if ! grep -qi 'End-to-end test DSA' "$out"; then
		echo "quipu-e2e: DSA description missing from showentry" >&2
		cat "$out" >&2 || true
		return 1
	fi

	echo "quipu-e2e: DISH read the test DSA:"
	sed -n '1,80p' "$out"
}

trap on_exit EXIT

find_binaries
prepare_workdir
start_dsa
run_dish
KEEP_WORKDIR=0
echo "quipu-e2e: ok"
