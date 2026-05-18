#!/bin/bash

echo "Определение пакетного менеджера..."

# Функция проверки наличия команды
has() {
    command -v "$1" &> /dev/null
}

# Пробуем найти пакетный менеджер
if has apt; then
    PM="apt"
    INSTALL="sudo apt update && sudo apt install -y"
    PKGS="build-essential cmake g++"
elif has dnf; then
    PM="dnf"
    INSTALL="sudo dnf install -y"
    PKGS="gcc-c++ cmake make"
elif has yum; then
    PM="yum"
    INSTALL="sudo yum install -y"
    PKGS="gcc-c++ cmake make"
elif has pacman; then
    PM="pacman"
    INSTALL="sudo pacman -S --noconfirm"
    PKGS="base-devel cmake gcc"
elif has zypper; then
    PM="zypper"
    INSTALL="sudo zypper install -y"
    PKGS="gcc-c++ cmake make"
elif has apk; then
    PM="apk"
    INSTALL="sudo apk add"
    PKGS="g++ cmake make"
elif has emerge; then
    PM="emerge"
    INSTALL="sudo emerge"
    PKGS="sys-devel/gcc dev-build/cmake"
elif has xbps-install; then
    PM="xbps"
    INSTALL="sudo xbps-install -y"
    PKGS="gcc cmake make"
elif has nix-env; then
    PM="nix"
    INSTALL="nix-env -i"
    PKGS="gcc cmake gnumake"
elif has brew; then
    PM="brew"
    INSTALL="brew install"
    PKGS="gcc cmake make"
else
    echo "Не удалось определить пакетный менеджер"
    echo "Установите вручную: g++, cmake, make"
    echo ""
    echo "Попробуйте:"
    echo "  Ubuntu/Debian: sudo apt install build-essential cmake g++"
    echo "  Fedora:        sudo dnf install gcc-c++ cmake make"
    echo "  Arch:          sudo pacman -S base-devel cmake gcc"
    echo "  openSUSE:      sudo zypper install gcc-c++ cmake make"
    echo "  Alpine:        sudo apk add g++ cmake make"
    echo "  Gentoo:        sudo emerge sys-devel/gcc dev-build/cmake"
    echo "  Void:          sudo xbps-install gcc cmake make"
    echo "  macOS:         brew install gcc cmake make"
    exit 1
fi

echo "Найден пакетный менеджер: $PM"
echo "Установка: $PKGS"
echo ""

eval "$INSTALL $PKGS"

if [ $? -eq 0 ]; then
    echo ""
    echo "Готово! Запустите ./build_server.sh"
else
    echo ""
    echo "Ошибка установки. Попробуйте вручную:"
    echo "  $INSTALL $PKGS"
    exit 1
fi
