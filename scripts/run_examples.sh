#!/bin/bash
echo "======================================================="
echo "================= Run SLOTHE examples ================="
echo "======================================================="

# Display menu
echo "[*] Select an option:"
echo "    1) Run Tanh T-40"
echo "    2) Run Tanh T-900"
echo "    3) Run Gelu T-40"
echo "    4) Run Gelu T-50"
echo "    5) Run Sigmoid T-40"
echo "    6) Run Sigmoid T-900"
echo "    7) Run Tanh E-3"
echo "    8) Run Tanh E-5"
echo "    0) Clean files"

read -p "Enter your choice: " choice

case "$choice" in
  1)
    echo "[*] Running Tanh T-40..."
    bash ./scripts/SLOTHE.sh tanh 3 40 -4 4 "minErr" 50
    ;;
  2)
    echo "[*] Running Tanh T-900..."
    bash ./scripts/SLOTHE.sh tanh 4 900 -4 4 "minErr" 50
    ;;
  3)
    echo "[*] Running Gelu T-40..."
    bash ./scripts/SLOTHE.sh gelu 2 40 -8 8 "minErr" 50
    ;;
  4)
    echo "[*] Running Gelu T-50..."
    bash ./scripts/SLOTHE.sh gelu 2 50 -8 8 "minErr" 50
    ;;
  5)
    echo "[*] Running Sigmoid T-40..."
    bash ./scripts/SLOTHE.sh sigmoid 2 40 -5 5 "minErr" 50
    ;;
  6)
    echo "[*] Running Sigmoid T-700..."
    bash ./scripts/SLOTHE.sh sigmoid 2 700 -5 5 "minErr" 50
    ;;
  7)
    echo "[*] Running Tanh E-3..."
    bash ./scripts/SLOTHE.sh tanh 3 40 -4 4 "minTime" 50
    ;;
  8)
    echo "[*] Running Tanh E-5..."
    bash ./scripts/SLOTHE.sh tanh 5 40 -4 4 "minTime" 50
    ;;
  0)
    echo "[*] Removing temporal files..."
    bash ./scripts/clean.sh
    ;;
  q|Q)
    echo "Exiting."
    exit 0
    ;;
  *)
    echo "[!] Invalid choice: $choice"
    exit 1
    ;;
esac
